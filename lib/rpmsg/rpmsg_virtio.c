/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (c) 2018 Linaro, Inc. All rights reserved.
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <metal/alloc.h>
#include <metal/sleep.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include <openamp/rpmsg_virtio.h>
#include <openamp/virtqueue.h>

#include "rpmsg_internal.h"

#define RPMSG_NUM_VRINGS                        2

/* Total tick count for 15secs - 1usec tick. */
#define RPMSG_TICK_COUNT                        15000000

/* Time to wait - In multiple of 1 msecs. */
#define RPMSG_TICKS_PER_INTERVAL                1000

/*
 * Get the buffer held counter value.
 * If 0 the buffer can be released
 */
#define RPMSG_BUF_HELD_COUNTER(rp_hdr)          \
	(((rp_hdr)->reserved & RPMSG_BUF_HELD_MASK) >> RPMSG_BUF_HELD_SHIFT)

/* Increase buffer held counter */
#define RPMSG_BUF_HELD_INC(rp_hdr)              \
	((rp_hdr)->reserved += 1 << RPMSG_BUF_HELD_SHIFT)

/* Decrease buffer held counter */
#define RPMSG_BUF_HELD_DEC(rp_hdr)              \
	((rp_hdr)->reserved -= 1 << RPMSG_BUF_HELD_SHIFT)

/* Get the buffer index */
#define RPMSG_BUF_INDEX(rphdr)                  \
	((uint16_t)((rp_hdr)->reserved & ~RPMSG_BUF_HELD_MASK))

/**
 * struct vbuff_reclaimer_t - vring buffer recycler
 *
 * This structure is used by the rpmsg virtio to store unused virtio buffer, as the
 * virtqueue structure has been already updated and memory allocated.
 *
 * @param node	node in reclaimer list.
 * @param idx	virtio descriptor index containing the buffer information.
 */
struct vbuff_reclaimer_t {
	struct metal_list node;
	uint16_t idx;
};

/* Default configuration */
#if VIRTIO_ENABLED(VIRTIO_DRIVER_SUPPORT)
#define RPMSG_VIRTIO_DEFAULT_CONFIG                \
	(&(const struct rpmsg_virtio_config) {     \
		.h2r_buf_size = RPMSG_BUFFER_SIZE, \
		.r2h_buf_size = RPMSG_BUFFER_SIZE, \
		.split_shpool = false,             \
	})
#else
#define RPMSG_VIRTIO_DEFAULT_CONFIG          NULL
#endif

#if VIRTIO_ENABLED(VIRTIO_DRIVER_SUPPORT)
metal_weak void *
rpmsg_virtio_shm_pool_get_buffer(struct rpmsg_virtio_shm_pool *shpool,
				 size_t size)
{
	void *buffer;

	if (!shpool || size == 0 || shpool->avail < size)
		return NULL;
	buffer = (char *)shpool->base + shpool->size - shpool->avail;
	shpool->avail -= size;

	return buffer;
}
#endif

void rpmsg_virtio_init_shm_pool(struct rpmsg_virtio_shm_pool *shpool,
				void *shb, size_t size)
{
	if (!shpool || !shb || size == 0)
		return;
	shpool->base = shb;
	shpool->size = size;
	shpool->avail = size;
}

/**
 * @internal
 *
 * @brief Places the used buffer back on the virtqueue.
 *
 * @param rvdev		Pointer to remote core
 * @param buffer	Buffer pointer
 * @param len		Buffer length
 * @param idx		Buffer index
 */
static void rpmsg_virtio_return_buffer(struct rpmsg_virtio_device *rvdev,
				       void *buffer, uint32_t len,
				       uint16_t idx)
{
	int ret;

	BUFFER_INVALIDATE(buffer, len);

	if (VIRTIO_ROLE_IS_DRIVER(rvdev->vdev)) {
		struct virtqueue_buf vqbuf;

		(void)idx;
		/* Initialize buffer node */
		vqbuf.buf = buffer;
		vqbuf.len = len;
		ret = virtqueue_add_buffer(rvdev->rvq, &vqbuf, 0, 1, buffer);
		RPMSG_ASSERT(ret == VQUEUE_SUCCESS, "add buffer failed\r\n");
	}

	if (VIRTIO_ROLE_IS_DEVICE(rvdev->vdev)) {
		(void)buffer;
		ret = virtqueue_add_consumed_buffer(rvdev->rvq, idx, len);
		RPMSG_ASSERT(ret == VQUEUE_SUCCESS, "add consumed buffer failed\r\n");
	}
}

/**
 * @internal
 *
 * @brief Places buffer on the virtqueue for consumption by the other side.
 *
 * @param rvdev		Pointer to rpmsg virtio
 * @param buffer	Buffer pointer
 * @param len		Buffer length
 * @param idx		Buffer index
 *
 * @return Status of function execution
 */
static int rpmsg_virtio_enqueue_buffer(struct rpmsg_virtio_device *rvdev,
				       void *buffer, uint32_t len,
				       uint16_t idx)
{
	BUFFER_FLUSH(buffer, len);

	if (VIRTIO_ROLE_IS_DRIVER(rvdev->vdev)) {
		struct virtqueue_buf vqbuf;
		(void)idx;

		/* Initialize buffer node */
		vqbuf.buf = buffer;
		vqbuf.len = len;
		return virtqueue_add_buffer(rvdev->svq, &vqbuf, 1, 0, buffer);
	}

	if (VIRTIO_ROLE_IS_DEVICE(rvdev->vdev)) {
		(void)buffer;
		return virtqueue_add_consumed_buffer(rvdev->svq, idx, len);
	}

	return 0;
}

/**
 * @internal
 *
 * @brief Provides buffer to transmit messages.
 *
 * @param rvdev	Pointer to rpmsg device
 * @param len	Length of returned buffer
 * @param idx	Buffer index
 *
 * @return Pointer to buffer.
 */
static void *rpmsg_virtio_get_tx_buffer(struct rpmsg_virtio_device *rvdev,
					uint32_t *len, uint16_t *idx)
{
	struct metal_list *node;
	struct vbuff_reclaimer_t *r_desc;
	void *data = NULL;

	/* Try first to recycle a buffer that has been freed without been used */
	node = metal_list_first(&rvdev->reclaimer);
	if (node) {
		r_desc = metal_container_of(node, struct vbuff_reclaimer_t, node);
		metal_list_del(node);
		data = r_desc;
		*idx = r_desc->idx;

		if (VIRTIO_ROLE_IS_DRIVER(rvdev->vdev))
			*len = rvdev->config.h2r_buf_size;
		if (VIRTIO_ROLE_IS_DEVICE(rvdev->vdev))
			*len = virtqueue_get_buffer_length(rvdev->svq, *idx);
	} else if (VIRTIO_ROLE_IS_DRIVER(rvdev->vdev)) {
		data = virtqueue_get_buffer(rvdev->svq, len, idx);
		if (!data && rvdev->svq->vq_free_cnt) {
			data = rpmsg_virtio_shm_pool_get_buffer(rvdev->shpool,
					rvdev->config.h2r_buf_size);
			*len = rvdev->config.h2r_buf_size;
			*idx = 0;
		}
	} else if (VIRTIO_ROLE_IS_DEVICE(rvdev->vdev)) {
		data = virtqueue_get_available_buffer(rvdev->svq, idx, len);
	}

	return data;
}

/**
 * @internal
 *
 * @brief Retrieves the received buffer from the virtqueue.
 *
 * @param rvdev	Pointer to rpmsg device
 * @param len	Size of received buffer
 * @param idx	Index of buffer
 *
 * @return Pointer to received buffer
 */
static void *rpmsg_virtio_get_rx_buffer(struct rpmsg_virtio_device *rvdev,
					uint32_t *len, uint16_t *idx)
{
	void *data = NULL;

	if (VIRTIO_ROLE_IS_DRIVER(rvdev->vdev)) {
		data = virtqueue_get_buffer(rvdev->rvq, len, idx);
	}

	if (VIRTIO_ROLE_IS_DEVICE(rvdev->vdev)) {
		data =
		    virtqueue_get_available_buffer(rvdev->rvq, idx, len);
	}

	/* Invalidate the buffer before returning it */
	if (data)
		BUFFER_INVALIDATE(data, *len);

	return data;
}

/**
 * @internal
 *
 * @brief Check if the remote is ready to start RPMsg communication
 *
 * @param rvdev Pointer to rpmsg_virtio device
 *
 * @return 0 on success, otherwise error code.
 */
static int rpmsg_virtio_wait_remote_ready(struct rpmsg_virtio_device *rvdev)
{
	uint8_t status;
	int ret;

	while (1) {
		ret = virtio_get_status(rvdev->vdev, &status);
		if (ret)
			return ret;
		/* Busy wait until the remote is ready */
		if (status & VIRTIO_CONFIG_STATUS_NEEDS_RESET) {
			ret = virtio_set_status(rvdev->vdev, 0);
			if (ret)
				return ret;
			/* TODO notify remote processor */
		} else if (status & VIRTIO_CONFIG_STATUS_DRIVER_OK) {
			return 0;
		}
		metal_yield();
	}
}

/**
 * @internal
 *
 * @brief Check whether rpmsg buffer needs to be released or not
 *
 * @param rp_hdr	Pointer to rpmsg buffer header
 *
 * @return true indicates this buffer needs to be released
 */
static bool rpmsg_virtio_buf_held_dec_test(struct rpmsg_hdr *rp_hdr)
{
	/* Check the held counter first */
	if (RPMSG_BUF_HELD_COUNTER(rp_hdr) <= 0) {
		metal_err("unexpected buffer held counter\r\n");
		return false;
	}

	/* Decrease the held counter */
	RPMSG_BUF_HELD_DEC(rp_hdr);

	/* Check whether to release the buffer */
	if (RPMSG_BUF_HELD_COUNTER(rp_hdr) > 0)
		return false;

	return true;
}

static void rpmsg_virtio_hold_rx_buffer(struct rpmsg_device *rdev, void *rxbuf)
{
	metal_mutex_acquire(&rdev->lock);
	RPMSG_BUF_HELD_INC(RPMSG_LOCATE_HDR(rxbuf));
	metal_mutex_release(&rdev->lock);
}

static bool rpmsg_virtio_release_rx_buffer_nolock(struct rpmsg_virtio_device *rvdev,
						  struct rpmsg_hdr *rp_hdr)
{
	uint16_t idx;
	uint32_t len;

	/* The reserved field contains buffer index */
	idx = RPMSG_BUF_INDEX(rp_hdr);
	/* Return buffer on virtqueue. */
	len = virtqueue_get_buffer_length(rvdev->rvq, idx);
	rpmsg_virtio_return_buffer(rvdev, rp_hdr, len, idx);

	return true;
}

static void rpmsg_virtio_release_rx_buffer(struct rpmsg_device *rdev,
					   void *rxbuf)
{
	struct rpmsg_virtio_device *rvdev;
	struct rpmsg_hdr *rp_hdr;

	rvdev = metal_container_of(rdev, struct rpmsg_virtio_device, rdev);
	rp_hdr = RPMSG_LOCATE_HDR(rxbuf);

	metal_mutex_acquire(&rdev->lock);
	if (rpmsg_virtio_buf_held_dec_test(rp_hdr)) {
		rpmsg_virtio_release_rx_buffer_nolock(rvdev, rp_hdr);
		/* Tell peer we returned an rx buffer */
		virtqueue_kick(rvdev->rvq);
	}
	metal_mutex_release(&rdev->lock);
}

static int rpmsg_virtio_notify_wait(struct rpmsg_virtio_device *rvdev, struct virtqueue *vq)
{
	struct virtio_vring_info *vring_info;

	vring_info = &rvdev->vdev->vrings_info[vq->vq_queue_index];

	if (!rvdev->notify_wait_cb)
		return RPMSG_EOPNOTSUPP;

	return rvdev->notify_wait_cb(&rvdev->rdev, vring_info->notifyid);
}

static void *rpmsg_virtio_get_tx_payload_buffer(struct rpmsg_device *rdev,
						uint32_t *len, int wait)
{
	struct rpmsg_virtio_device *rvdev;
	struct rpmsg_hdr *rp_hdr;
	uint8_t virtio_status;
	uint16_t idx;
	int tick_count;
	int status;

	/* Get the associated remote device for channel. */
	rvdev = metal_container_of(rdev, struct rpmsg_virtio_device, rdev);

	/* Validate device state */
	status = virtio_get_status(rvdev->vdev, &virtio_status);
	if (status || !(virtio_status & VIRTIO_CONFIG_STATUS_DRIVER_OK))
		return NULL;

	if (wait)
		tick_count = RPMSG_TICK_COUNT / RPMSG_TICKS_PER_INTERVAL;
	else
		tick_count = 0;

	while (1) {
		/* Lock the device to enable exclusive access to virtqueues */
		metal_mutex_acquire(&rdev->lock);
		rp_hdr = rpmsg_virtio_get_tx_buffer(rvdev, len, &idx);
		metal_mutex_release(&rdev->lock);
		if (rp_hdr || !tick_count)
			break;

		/*
		 * Try to use wait loop implemented in the virtio dispatcher and
		 * use metal_sleep_usec() method by default.
		 */
		status = rpmsg_virtio_notify_wait(rvdev, rvdev->rvq);
		if (status == RPMSG_EOPNOTSUPP) {
			metal_sleep_usec(RPMSG_TICKS_PER_INTERVAL);
			tick_count--;
		} else if (status != RPMSG_SUCCESS) {
			break;
		}
	}

	if (!rp_hdr)
		return NULL;

	/* Store the index into the reserved field to be used when sending */
	rp_hdr->reserved = idx;

	/* Increase the held counter to hold this Tx buffer */
	RPMSG_BUF_HELD_INC(rp_hdr);

	/* Actual data buffer size is vring buffer size minus header length */
	*len -= sizeof(struct rpmsg_hdr);
	return RPMSG_LOCATE_DATA(rp_hdr);
}

static int rpmsg_virtio_send_offchannel_nocopy(struct rpmsg_device *rdev,
					       uint32_t src, uint32_t dst,
					       const void *data, int len)
{
	struct rpmsg_virtio_device *rvdev;
	struct metal_io_region *io;
	struct rpmsg_hdr rp_hdr;
	struct rpmsg_hdr *hdr;
	uint32_t buff_len;
	uint16_t idx;
	int status;

	/* Get the associated remote device for channel. */
	rvdev = metal_container_of(rdev, struct rpmsg_virtio_device, rdev);

	hdr = RPMSG_LOCATE_HDR(data);
	/* The reserved field contains buffer index */
	idx = hdr->reserved;

	/* Initialize RPMSG header. */
	rp_hdr.dst = dst;
	rp_hdr.src = src;
	rp_hdr.len = len;
	rp_hdr.reserved = 0;
	rp_hdr.flags = 0;

	/* Copy data to rpmsg buffer. */
	io = rvdev->shbuf_io;
	status = metal_io_block_write(io, metal_io_virt_to_offset(io, hdr),
				      &rp_hdr, sizeof(rp_hdr));
	RPMSG_ASSERT(status == sizeof(rp_hdr), "failed to write header\r\n");

	metal_mutex_acquire(&rdev->lock);

	if (VIRTIO_ROLE_IS_DRIVER(rvdev->vdev))
		buff_len = rvdev->config.h2r_buf_size;
	else
		buff_len = virtqueue_get_buffer_length(rvdev->svq, idx);

	/* Enqueue buffer on virtqueue. */
	status = rpmsg_virtio_enqueue_buffer(rvdev, hdr, buff_len, idx);
	RPMSG_ASSERT(status == VQUEUE_SUCCESS, "failed to enqueue buffer\r\n");
	/* Let the other side know that there is a job to process. */
	virtqueue_kick(rvdev->svq);

	metal_mutex_release(&rdev->lock);

	return len;
}

static int rpmsg_virtio_release_tx_buffer(struct rpmsg_device *rdev, void *txbuf)
{
	struct rpmsg_virtio_device *rvdev;
	struct rpmsg_hdr *rp_hdr = RPMSG_LOCATE_HDR(txbuf);
	void *vbuff = rp_hdr;  /* only used to avoid warning on the cast of a packed structure */
	struct vbuff_reclaimer_t *r_desc = (struct vbuff_reclaimer_t *)vbuff;

	rvdev = metal_container_of(rdev, struct rpmsg_virtio_device, rdev);

	metal_mutex_acquire(&rdev->lock);

	/* Check whether to release the Tx buffer */
	if (rpmsg_virtio_buf_held_dec_test(rp_hdr)) {
		/*
		 * Reuse the RPMsg buffer to temporary store the vbuff_reclaimer_t structure.
		 * Store the index locally before overwriting the RPMsg header.
		 */
		r_desc->idx = RPMSG_BUF_INDEX(rp_hdr);
		metal_list_add_tail(&rvdev->reclaimer, &r_desc->node);
	}

	metal_mutex_release(&rdev->lock);

	return RPMSG_SUCCESS;
}

/**
 * @internal
 *
 * @brief This function sends rpmsg "message" to remote device.
 *
 * @param rdev	Pointer to rpmsg device
 * @param src	Source address of channel
 * @param dst	Destination address of channel
 * @param data	Data to transmit
 * @param len	Size of data
 * @param wait	Boolean, wait or not for buffer to become
 *		available
 *
 * @return Size of data sent or negative value for failure.
 */
static int rpmsg_virtio_send_offchannel_raw(struct rpmsg_device *rdev,
					    uint32_t src, uint32_t dst,
					    const void *data,
					    int len, int wait)
{
	struct rpmsg_virtio_device *rvdev;
	struct metal_io_region *io;
	uint32_t buff_len;
	void *buffer;
	int status;

	/* Get the associated remote device for channel. */
	rvdev = metal_container_of(rdev, struct rpmsg_virtio_device, rdev);

	/* Get the payload buffer. */
	buffer = rpmsg_virtio_get_tx_payload_buffer(rdev, &buff_len, wait);
	if (!buffer)
		return RPMSG_ERR_NO_BUFF;

	/* Copy data to rpmsg buffer. */
	if (len > (int)buff_len)
		len = buff_len;
	io = rvdev->shbuf_io;
	status = metal_io_block_write(io, metal_io_virt_to_offset(io, buffer),
				      data, len);
	RPMSG_ASSERT(status == len, "failed to write buffer\r\n");

	return rpmsg_virtio_send_offchannel_nocopy(rdev, src, dst, buffer, len);
}

/**
 * @internal
 *
 * @brief Tx callback function.
 *
 * @param vq	Pointer to virtqueue on which Tx is has been
 *		completed.
 */
static void rpmsg_virtio_tx_callback(struct virtqueue *vq)
{
	(void)vq;
}

/**
 * @internal
 *
 * @brief Rx callback function.
 *
 * @param vq	Pointer to virtqueue on which messages is received
 */
static void rpmsg_virtio_rx_callback(struct virtqueue *vq)
{
	struct virtio_device *vdev = vq->vq_dev;
	struct rpmsg_virtio_device *rvdev = vdev->priv;
	struct rpmsg_device *rdev = &rvdev->rdev;
	struct rpmsg_endpoint *ept;
	struct rpmsg_hdr *rp_hdr;
	bool release = false;
	uint32_t len;
	uint16_t idx;
	int status;

	while (1) {
		/* Process the received data from remote node */
		metal_mutex_acquire(&rdev->lock);
		rp_hdr = rpmsg_virtio_get_rx_buffer(rvdev, &len, &idx);

		/* No more filled rx buffers */
		if (!rp_hdr) {
			if (VIRTIO_ENABLED(VQ_RX_EMPTY_NOTIFY) && release)
				/* Tell peer we returned some rx buffer */
				virtqueue_kick(rvdev->rvq);
			metal_mutex_release(&rdev->lock);
			break;
		}

		rp_hdr->reserved = idx;

		/* Get the channel node from the remote device channels list. */
		ept = rpmsg_get_ept_from_addr(rdev, rp_hdr->dst);
		rpmsg_ept_incref(ept);
		RPMSG_BUF_HELD_INC(rp_hdr);
		metal_mutex_release(&rdev->lock);

		if (ept) {
			if (ept->dest_addr == RPMSG_ADDR_ANY) {
				/*
				 * First message received from the remote side,
				 * update channel destination address
				 */
				ept->dest_addr = rp_hdr->src;
			}
			status = ept->cb(ept, RPMSG_LOCATE_DATA(rp_hdr),
					 rp_hdr->len, rp_hdr->src, ept->priv);

			RPMSG_ASSERT(status >= 0,
				     "unexpected callback status\r\n");
		}

		metal_mutex_acquire(&rdev->lock);
		rpmsg_ept_decref(ept);
		if (rpmsg_virtio_buf_held_dec_test(rp_hdr)) {
			rpmsg_virtio_release_rx_buffer_nolock(rvdev, rp_hdr);
			if (VIRTIO_ENABLED(VQ_RX_EMPTY_NOTIFY))
				/* Kick will be sent only when last buffer is released */
				release = true;
			else
				/* Tell peer we returned an rx buffer */
				virtqueue_kick(rvdev->rvq);
		}
		metal_mutex_release(&rdev->lock);
	}
}

/**
 * @internal
 *
 * @brief This callback handles name service announcement from the remote
 * device and creates/deletes rpmsg channels.
 *
 * @param ept	Pointer to server channel control block.
 * @param data	Pointer to received messages
 * @param len	Length of received data
 * @param priv	Any private data
 * @param src	Source address
 *
 * @return Rpmsg endpoint callback handled
 */
static int rpmsg_virtio_ns_callback(struct rpmsg_endpoint *ept, void *data,
				    size_t len, uint32_t src, void *priv)
{
	struct rpmsg_device *rdev = priv;
	struct rpmsg_virtio_device *rvdev = metal_container_of(rdev,
							       struct rpmsg_virtio_device,
							       rdev);
	struct metal_io_region *io = rvdev->shbuf_io;
	struct rpmsg_endpoint *_ept;
	struct rpmsg_ns_msg *ns_msg;
	uint32_t dest;
	bool ept_to_release;
	char name[RPMSG_NAME_SIZE];

	(void)ept;
	(void)src;

	ns_msg = data;
	if (len != sizeof(*ns_msg))
		/* Returns as the message is corrupted */
		return RPMSG_SUCCESS;
	metal_io_block_read(io,
			    metal_io_virt_to_offset(io, ns_msg->name),
			    &name, sizeof(name));
	dest = ns_msg->addr;

	/* check if a Ept has been locally registered */
	metal_mutex_acquire(&rdev->lock);
	_ept = rpmsg_get_endpoint(rdev, name, RPMSG_ADDR_ANY, dest);

	/*
	 * If ept-release callback is not implemented, ns_unbind_cb() can free the ept.
	 * Test _ept->release_cb before calling ns_unbind_cb() callbacks.
	 */
	ept_to_release = _ept && _ept->release_cb;

	if (ns_msg->flags & RPMSG_NS_DESTROY) {
		if (_ept)
			_ept->dest_addr = RPMSG_ADDR_ANY;
		if (ept_to_release)
			rpmsg_ept_incref(_ept);
		metal_mutex_release(&rdev->lock);
		if (_ept && _ept->ns_unbind_cb)
			_ept->ns_unbind_cb(_ept);
		if (rdev->ns_unbind_cb)
			rdev->ns_unbind_cb(rdev, name, dest);
		if (ept_to_release) {
			metal_mutex_acquire(&rdev->lock);
			rpmsg_ept_decref(_ept);
			metal_mutex_release(&rdev->lock);
		}
	} else {
		if (!_ept) {
			/*
			 * send callback to application, that can
			 * - create the associated endpoints.
			 * - store information for future use.
			 * - just ignore the request as service not supported.
			 */
			metal_mutex_release(&rdev->lock);
			if (rdev->ns_bind_cb)
				rdev->ns_bind_cb(rdev, name, dest);
		} else {
			_ept->dest_addr = dest;
			metal_mutex_release(&rdev->lock);
		}
	}

	return RPMSG_SUCCESS;
}

int rpmsg_virtio_get_tx_buffer_size(struct rpmsg_device *rdev)
{
	struct rpmsg_virtio_device *rvdev;
	int size = 0;

	if (!rdev)
		return RPMSG_ERR_PARAM;

	metal_mutex_acquire(&rdev->lock);
	rvdev = (struct rpmsg_virtio_device *)rdev;

	if (VIRTIO_ROLE_IS_DRIVER(rvdev->vdev)) {
		/*
		 * If device role is host then buffers are provided by us,
		 * so just provide the macro.
		 */
		size = rvdev->config.h2r_buf_size - sizeof(struct rpmsg_hdr);
	}

	if (VIRTIO_ROLE_IS_DEVICE(rvdev->vdev)) {
		/*
		 * If other core is host then buffers are provided by it,
		 * so get the buffer size from the virtqueue.
		 */
		size = (int)virtqueue_get_desc_size(rvdev->svq) -
		       sizeof(struct rpmsg_hdr);
	}

	if (size <= 0)
		size = RPMSG_ERR_NO_BUFF;

	metal_mutex_release(&rdev->lock);

	return size;
}

int rpmsg_virtio_get_rx_buffer_size(struct rpmsg_device *rdev)
{
	struct rpmsg_virtio_device *rvdev;
	int size = 0;

	if (!rdev)
		return RPMSG_ERR_PARAM;

	metal_mutex_acquire(&rdev->lock);
	rvdev = (struct rpmsg_virtio_device *)rdev;

	if (VIRTIO_ROLE_IS_DRIVER(rvdev->vdev)) {
		/*
		 * If device role is host then buffers are provided by us,
		 * so just provide the macro.
		 */
		size = rvdev->config.r2h_buf_size - sizeof(struct rpmsg_hdr);
	}

	if (VIRTIO_ROLE_IS_DEVICE(rvdev->vdev)) {
		/*
		 * If other core is host then buffers are provided by it,
		 * so get the buffer size from the virtqueue.
		 */
		size = (int)virtqueue_get_desc_size(rvdev->rvq) -
		       sizeof(struct rpmsg_hdr);
	}

	if (size <= 0)
		size = RPMSG_ERR_NO_BUFF;

	metal_mutex_release(&rdev->lock);

	return size;
}

int rpmsg_init_vdev(struct rpmsg_virtio_device *rvdev,
		    struct virtio_device *vdev,
		    rpmsg_ns_bind_cb ns_bind_cb,
		    struct metal_io_region *shm_io,
		    struct rpmsg_virtio_shm_pool *shpool)
{
	return rpmsg_init_vdev_with_config(rvdev, vdev, ns_bind_cb, shm_io,
			   shpool, RPMSG_VIRTIO_DEFAULT_CONFIG);
}

int rpmsg_init_vdev_with_config(struct rpmsg_virtio_device *rvdev,
				struct virtio_device *vdev,
				rpmsg_ns_bind_cb ns_bind_cb,
				struct metal_io_region *shm_io,
				struct rpmsg_virtio_shm_pool *shpool,
				const struct rpmsg_virtio_config *config)
{
	struct rpmsg_device *rdev;
	const char *vq_names[RPMSG_NUM_VRINGS];
	vq_callback callback[RPMSG_NUM_VRINGS];
	uint32_t features;
	int status;
	unsigned int i;

	if (!rvdev || !vdev || !shm_io)
		return RPMSG_ERR_PARAM;

	rdev = &rvdev->rdev;
	rvdev->notify_wait_cb = NULL;
	memset(rdev, 0, sizeof(*rdev));
	metal_mutex_init(&rdev->lock);
	rvdev->vdev = vdev;
	rdev->ns_bind_cb = ns_bind_cb;
	vdev->priv = rvdev;
	rdev->ops.send_offchannel_raw = rpmsg_virtio_send_offchannel_raw;
	rdev->ops.hold_rx_buffer = rpmsg_virtio_hold_rx_buffer;
	rdev->ops.release_rx_buffer = rpmsg_virtio_release_rx_buffer;
	rdev->ops.get_tx_payload_buffer = rpmsg_virtio_get_tx_payload_buffer;
	rdev->ops.send_offchannel_nocopy = rpmsg_virtio_send_offchannel_nocopy;
	rdev->ops.release_tx_buffer = rpmsg_virtio_release_tx_buffer;
	rdev->ops.get_rx_buffer_size = rpmsg_virtio_get_rx_buffer_size;
	rdev->ops.get_tx_buffer_size = rpmsg_virtio_get_tx_buffer_size;

	if (VIRTIO_ROLE_IS_DRIVER(vdev)) {
		/*
		 * The virtio configuration contains only options applicable to
		 * a virtio driver, implying rpmsg host role.
		 */
		if (config == NULL) {
			return RPMSG_ERR_PARAM;
		}
		rvdev->config = *config;
	}

	if (VIRTIO_ROLE_IS_DEVICE(vdev)) {
		/* wait synchro with the host */
		status = rpmsg_virtio_wait_remote_ready(rvdev);
		if (status)
			return status;
	}

	status = virtio_get_features(vdev, &features);
	if (status)
		return status;
	rdev->support_ns = !!(features & (1 << VIRTIO_RPMSG_F_NS));

	if (VIRTIO_ROLE_IS_DRIVER(vdev)) {
		/*
		 * Since device is RPMSG Remote so we need to manage the
		 * shared buffers. Create shared memory pool to handle buffers.
		 */
		rvdev->shpool = config->split_shpool ? shpool + 1 : shpool;
		if (!shpool)
			return RPMSG_ERR_PARAM;
		if (!shpool->size || !rvdev->shpool->size)
			return RPMSG_ERR_NO_BUFF;

		vq_names[0] = "rx_vq";
		vq_names[1] = "tx_vq";
		callback[0] = rpmsg_virtio_rx_callback;
		callback[1] = rpmsg_virtio_tx_callback;
	}

	if (VIRTIO_ROLE_IS_DEVICE(vdev)) {
		vq_names[0] = "tx_vq";
		vq_names[1] = "rx_vq";
		callback[0] = rpmsg_virtio_tx_callback;
		callback[1] = rpmsg_virtio_rx_callback;
	}

	rvdev->shbuf_io = shm_io;
	metal_list_init(&rvdev->reclaimer);

	/* Create virtqueues for remote device */
	status = virtio_create_virtqueues(vdev, 0, RPMSG_NUM_VRINGS,
					  vq_names, callback, NULL);
	if (status != RPMSG_SUCCESS)
		return status;

	/* Create virtqueue success, assign back the virtqueue */
	if (VIRTIO_ROLE_IS_DRIVER(vdev)) {
		rvdev->rvq  = vdev->vrings_info[0].vq;
		rvdev->svq  = vdev->vrings_info[1].vq;
	}

	if (VIRTIO_ROLE_IS_DEVICE(vdev)) {
		rvdev->rvq  = vdev->vrings_info[1].vq;
		rvdev->svq  = vdev->vrings_info[0].vq;
	}

	/*
	 * Suppress "tx-complete" interrupts
	 * since send method use busy loop when buffer pool exhaust
	 */
	virtqueue_disable_cb(rvdev->svq);

	/* TODO: can have a virtio function to set the shared memory I/O */
	for (i = 0; i < RPMSG_NUM_VRINGS; i++) {
		struct virtqueue *vq;

		vq = vdev->vrings_info[i].vq;
		vq->shm_io = shm_io;
	}

	if (VIRTIO_ROLE_IS_DRIVER(vdev)) {
		struct virtqueue_buf vqbuf;
		unsigned int idx;
		void *buffer;

		vqbuf.len = rvdev->config.r2h_buf_size;
		for (idx = 0; idx < rvdev->rvq->vq_nentries; idx++) {
			/* Initialize TX virtqueue buffers for remote device */
			buffer = rpmsg_virtio_shm_pool_get_buffer(shpool,
					rvdev->config.r2h_buf_size);

			if (!buffer) {
				status = RPMSG_ERR_NO_BUFF;
				goto err;
			}

			vqbuf.buf = buffer;

			metal_io_block_set(shm_io,
					   metal_io_virt_to_offset(shm_io,
								   buffer),
					   0x00, rvdev->config.r2h_buf_size);
			status =
				virtqueue_add_buffer(rvdev->rvq, &vqbuf, 0, 1,
						     buffer);

			if (status != RPMSG_SUCCESS) {
				goto err;
			}
		}
	}

	/* Initialize channels and endpoints list */
	metal_list_init(&rdev->endpoints);

	/*
	 * Create name service announcement endpoint if device supports name
	 * service announcement feature.
	 */
	if (rdev->support_ns) {
		rpmsg_register_endpoint(rdev, &rdev->ns_ept, "NS",
				     RPMSG_NS_EPT_ADDR, RPMSG_NS_EPT_ADDR,
				     rpmsg_virtio_ns_callback, NULL, rvdev);
	}

	if (VIRTIO_ROLE_IS_DRIVER(vdev)) {
		status = virtio_set_status(vdev, VIRTIO_CONFIG_STATUS_DRIVER_OK);
		if (status)
			goto err;
	}

	return RPMSG_SUCCESS;

err:
	virtio_delete_virtqueues(vdev);
	return status;
}

void rpmsg_deinit_vdev(struct rpmsg_virtio_device *rvdev)
{
	struct metal_list *node;
	struct rpmsg_device *rdev;
	struct rpmsg_endpoint *ept;

	if (rvdev) {
		rdev = &rvdev->rdev;
		while (!metal_list_is_empty(&rdev->endpoints)) {
			node = rdev->endpoints.next;
			ept = metal_container_of(node, struct rpmsg_endpoint, node);
			rpmsg_destroy_ept(ept);
		}

		rvdev->rvq = 0;
		rvdev->svq = 0;

		virtio_delete_virtqueues(rvdev->vdev);
		metal_mutex_deinit(&rdev->lock);
		rvdev->vdev = NULL;
	}
}
