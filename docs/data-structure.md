Remoteproc data struct
===========================
* Representation of the remote processor instance:

```
struct remoteproc {
	metal_mutex_t lock;                /**< mutex lock */
	void *rsc_table;                   /**< address of the resource table */
	size_t rsc_len;                    /**< length of the resource table */
	struct metal_io_region *rsc_io;    /**< metal I/O region of resource table*/
	struct metal_list mems;            /**< remoteproc memories */
	struct metal_list vdevs;           /**< remoteproc virtio devices */
	unsigned long bitmap;              /**< bitmap for notify IDs for remoteproc subdevices */
	const struct remoteproc_ops *ops;  /**< pointer to remoteproc operation */
	metal_phys_addr_t bootaddr;        /**< boot address */
	const struct loader_ops *loader;   /**< image loader operation */
	unsigned int state;                /**< remote processor state */
	void *priv;                        /**< remoteproc private data */
};
```
* Representation of the remote processor virtio device:
```

struct remoteproc_virtio {
	void *priv;                           /**< private data */
	void *vdev_rsc;                       /**< address of vdev resource */
	struct metal_io_region *vdev_rsc_io;  /**< metal I/O region of vdev_info, can be NULL */
	rpvdev_notify_func notify;            /**< notification function */
	struct virtio_device vdev;            /**< associated virtio device */
	struct metal_list node;               /**< node for the remoteproc vdevs list */
};

```

Virtio Data struct
===========================
* Representation of a virtio device:
```
struct virtio_dev {
	uint32_t notifyid;                       /**< unique position on the virtio bus */
	struct virtio_device_id id;              /**< the device type identification (used to match it with a driver */
	uint64_t features;                       /**< the features supported by both ends. */
	unsigned int role;                       /**< if it is virtio backend or front end. */
	virtio_dev_reset_cb reset_cb;            /**< user registered device callback */
	const struct virtio_dispatch *func;      /**< Virtio dispatch table */
	void *priv;                              /**< pointer to virtio_device private data */
	unsigned int vrings_num;                 /**< number of vrings */
	struct virtio_vring_info *vrings_info;   /**< vrings associated to the virtio device*/
};
```

* Representation of a virtqueue local context:
```
struct virtqueue {
	struct virtio_device *vq_dev;            /**< pointer to virtio device */
	const char *vq_name;                     /**< virtqueue name */
	uint16_t vq_queue_index;                 /**< virtqueue name */
	uint16_t vq_nentries;
	void (*callback)(struct virtqueue *vq);  /**< virtqueue callback */
	void (*notify)(struct virtqueue *vq);    /**< virtqueue notify remote function */
	struct vring vq_ring;
	uint16_t vq_free_cnt;
	uint16_t vq_queued_cnt;
	void *shm_io;                            /**< pointer to the shared buffer I/O region */

	/*
	 * Head of the free chain in the descriptor table. If
	 * there are no free descriptors, this will be set to
	 * VQ_RING_DESC_CHAIN_END.
	 */
	uint16_t vq_desc_head_idx;

	/*
	 * Last consumed descriptor in the used table,
	 * trails vq_ring.used->idx.
	 */
	uint16_t vq_used_cons_idx;

	/*
	 * Last consumed descriptor in the available table -
	 * used by the consumer side.
	 */
	uint16_t vq_available_idx;

	/*
	 * Used by the host side during callback. Cookie
	 * holds the address of buffer received from other side.
	 * Other fields in this structure are not used currently.
	 * Do we needed??/
	struct vq_desc_extra {
		void *cookie;
		struct vring_desc *indirect;
		uint32_t indirect_paddr;
		uint16_t ndescs;
	} vq_descx[0];
};
```

* Representation of a shared virtqueue structure defined in Virtual I/O Device (VIRTIO) Version 1.1:
```
struct vring {
	unsigned int num;           /**< number of buffers of the vring */
	struct vring_desc *desc;    /**< pointer to the buffers descriptor */
	struct vring_avail *avail;  /**< pointer to the ring of available descriptor heads*/
	struct vring_used *used;    /**< pointer to the ring of used descriptor heads */
};
```
RPMsg virtio Data struct
===========================
* Representation of a RPMsg virtio device:
```
struct rpmsg_virtio_device {
	struct rpmsg_device rdev;               /**< the associated rpmsg device */
	struct rpmsg_virtio_config config;      /**< structure containing the virtio configuration */
	struct virtio_device *vdev;             /**< pointer to the virtio device */
	struct virtqueue *rvq;                  /**< pointer to the receive virtqueue */
	struct virtqueue *svq;                  /**< a to the send virtqueue */
	struct metal_io_region *shbuf_io;       /**< pointer to the shared buffer I/O region */
	struct rpmsg_virtio_shm_pool *shpool;   /**< pointer to the shared buffers pool */
};

```

RPMsg Data struct
===========================
* representation of a RPMsg devices:
```
struct rpmsg_device {
	struct metal_list endpoints;                                   /**< list of endpoints */
	struct rpmsg_endpoint ns_ept;                                  /**< name service endpoint */
	unsigned long bitmap[metal_bitmap_longs(RPMSG_ADDR_BMP_SIZE)]; /**< bitmap: table endpoint address allocation */
	metal_mutex_t lock;                                            /**<  mutex lock for rpmsg management */
	rpmsg_ns_bind_cb ns_bind_cb;                                   /**< callback handler for name service announcement without local endpoints waiting to bind. */
	struct rpmsg_device_ops ops;                                   /**<  RPMsg device operations */
	bool support_ns;                                               /**< create/destroy namespace message */
};

* Representation of a local RPMsg endpoint associated to an unique address:
struct rpmsg_endpoint {
	char name[SERVICE_NAME_SIZE];                                                     /**< associated name service */
	struct rpmsg_virtio_dev *rvdev;                                                   /**< pointer to the RPMsg virtio device */
	uint32_t addr;                                                                    /**< endpoint local address */
	uint32_t dest_addr;                                                               /**< endpoint default target address */
	int (*cb)(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t addr);     /**< endpoint callback */
	void (*ns_unbind_cb)(struct rpmsg_endpoint *ept);                                 /**< remote endpoint destroy callback */
	struct metal_list node;                                                           /**< node for the rpmsg_device endpoints list */
	void *priv;                                                                       /**< user private data */
};
```
