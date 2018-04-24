# Remoteproc Design Document
## Remoteproc LCM States
![Rproc LCM States](img/rproc-lcm-state-machine.png)

### State Transition
| State Transition | Transition Trigger |
|:-----------------|:-------------------|
| Offline -> Ready | load firmware ;<br>`remoteproc_load_fw(&rproc, fw, &loader)` |
| Ready -> Running | start the processor; <br>`remoteproc_start(&rproc)` |
| Ready -> Stopped | stop the processor; <br>`remoteproc_stop(&rproc)`; <br>`remoteproc_shutdown(&rproc)` |
| Running -> Error | remote error has been detected; <br>`remoteproc_error_cb(&rproc)` |
| Running -> Stopped | stop the processor; <br>`remoteproc_stop(&rproc)`; <br>`remoteproc_shutdown(&rproc)` |
| Runinng -> Suspended | suspend the processor; <br>`remoteproc_suspend(&rproc, suspend_level)`|
| Suspended -> Running | wakeup callback is called from the remoteproc instance; <br>`remoteproc_wakenup_cb(&rproc)` |
| Suspended -> Stopped | stop the processor; <br>`remoteproc_stop(&rproc)`; <br>`remoteproc_shutdown(&rproc)` |
| Error -> Stopped |stop the processor; <br>`remoteproc_stop(&rproc)`; <br>`remoteproc_shutdown(&rproc)` |
| Stopped -> Offline | shutdown the processor; `remoteproc_shutdown(&rproc)` |

###

