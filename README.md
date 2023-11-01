# Non-Volatile Storage (NVS) Read and Write Example

This illustrates an issue of writng multiple blobs to the NVS.

## How to use example

### Hardware required

ESP32-S3

## Example Output

```
I (350) main_task: Calling app_main()
Nothing saved yet!
I (540) storage: Saving blob, pass 1
Writing : 6000
Writing : 4000
Blob size : 6000
I (700) storage: Saving blob, pass 2
Writing : 6000
Writing : 4000
Blob size : 6000
I (860) storage: Saving blob, pass 3
Writing : 6000

[ ... ]

Writing : 6000
Writing : 4000
Blob size : 6000
I (12260) storage: Saving blob, pass 79
Writing : 6000
Writing : 4000
Blob size : 6000
I (12410) storage: Saving blob, pass 80
Writing : 6000
Writing : 4000
W (12410) storage: Error (ESP_ERR_NVS_NOT_ENOUGH_SPACE) saving run time blob to NVS!
Blob size : 6000
```
