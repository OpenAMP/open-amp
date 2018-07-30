# Remoteproc Design Document
Remoteproc provides abstraction to manage the life cycle of a remote
application. For now, it only provides APIs on bringing up and
tearing down the remote application, and parsing resource table.
It will extend to crash detection, suspend and resume.

## Remoteproc LCM States
| State | State Description |
|:------|:------------------|
| Offline | Initial state of a remoteproc instance. The remote presented by the remoteproc instance and its resource has been powered off. |
| Configured | The remote presented by the remoteproc instance has been configured. And ready to load applicaiton. |
| Ready | The remote presented by the remoteproc instance has applicaiton loaded, and ready to run. |
| Stopped | The remote presented by the remoteproc instance has stopped from running. But the remote is still powered on. And the remote's resource hasn't been released. |

![Rproc LCM States](img/rproc-lcm-state-machine.png)

### State Transition
| State Transition | Transition Trigger |
|:-----------------|:-------------------|
| Offline -> Configured | Configure the remote to make it able to load application;<br>`remoteproc_configure(&rproc, &config_data)`|
| Configured -> Ready | load firmware ;<br>`remoteproc_load(&rproc, &path, &image_store, &image_store_ops, &image_info)` |
| Ready -> Running | start the processor; <br>`remoteproc_start(&rproc)` |
| Ready -> Stopped | stop the processor; <br>`remoteproc_stop(&rproc)`; <br>`remoteproc_shutdown(&rproc)`(Stopped is the intermediate state of shutdown operation)  |
| Running -> Stopped | stop the processor; <br>`remoteproc_stop(&rproc)`; <br>`remoteproc_shutdown(&rproc)` |
| Stopped -> Offline | shutdown the processor; <br>`remoteproc_shutdown(&rproc)` |

###

