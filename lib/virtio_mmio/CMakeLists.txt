if (WITH_VIRTIO_MMIO_DRV)
collect (PROJECT_LIB_SOURCES virtio_mmio_drv.c)
collect (PROJECT_LIB_SOURCES virtio_rng_drv.c)
collect (PROJECT_LIB_SOURCES virtio_net_drv.c)
collect (PROJECT_LIB_SOURCES virtio_serial_drv.c)
endif (WITH_VIRTIO_MMIO_DRV)

if (WITH_HVL_VIRTIO_DRV)
collect (PROJECT_LIB_SOURCES virtio_mmio_hvl.c)
endif (WITH_HVL_VIRTIO_DRV)

if (WITH_VIRTIO_MMIO_DEV)
collect (PROJECT_LIB_SOURCES virtio_mmio_dev.c)
endif (WITH_VIRTIO_MMIO_DEV)
