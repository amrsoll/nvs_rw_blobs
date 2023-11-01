/* Non-Volatile Storage (NVS) Read and Write a Blob - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

#define STORAGE_NAMESPACE "storage"
#define STORAGE_BLOB_1 "storage_blob_1"
#define STORAGE_BLOB_2 "storage_blob_2"

#if CONFIG_IDF_TARGET_ESP32C3
#define BOOT_MODE_PIN GPIO_NUM_9
#else
#define BOOT_MODE_PIN GPIO_NUM_0
#endif //CONFIG_IDF_TARGET_ESP32C3

#define BUFFER_LEN_1 6000
#define BUFFER_LEN_2 4000


/* Save new run time value in NVS
   by first reading a table of previously saved values
   and then adding the new value at the end of the table.
   Return an error if anything goes wrong
   during this process.
 */
esp_err_t save_blobs(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Read previously saved blob if available
    // uint32_t* my_blob = malloc(required_size);
    char * my_blob = malloc(BUFFER_LEN_1);

    // Write value including previously saved blob if available
    printf("Writing : %zu\n", BUFFER_LEN_1 * sizeof(char));
    err = nvs_set_blob(my_handle, STORAGE_BLOB_1, my_blob, BUFFER_LEN_1 * sizeof(char));
    my_blob = realloc(my_blob, BUFFER_LEN_2 * sizeof(char));
    printf("Writing : %zu\n", BUFFER_LEN_2 * sizeof(char));
    err = nvs_set_blob(my_handle, STORAGE_BLOB_2, my_blob, BUFFER_LEN_2 * sizeof(char));

    if (err != ESP_OK) return err;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}

/* Read from NVS and print restart counter
   and the table with run times.
   Return an error if anything goes wrong
   during this process.
 */
esp_err_t read_blob_size(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Read restart counter
    int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(my_handle, "restart_conter", &restart_counter);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    // Read run time blob
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    // obtain required memory space to store blob being read from NVS
    err = nvs_get_blob(my_handle, STORAGE_BLOB_1, NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    if (required_size == 0) {
        printf("Nothing saved yet!\n");
    } else {
        char* my_blob = malloc(required_size);
        err = nvs_get_blob(my_handle, STORAGE_BLOB_1, my_blob, &required_size);
        if (err != ESP_OK) {
            free(my_blob);
            nvs_close(my_handle);
            return err;
        }
        printf("Blob size : %zu\n", required_size);
        free(my_blob);
    }

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}


void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_erase());
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );


    // gpio_reset_pin(BOOT_MODE_PIN);
    // gpio_set_direction(BOOT_MODE_PIN, GPIO_MODE_INPUT);

    /* Read the status of GPIO0. If GPIO0 is LOW for longer than 1000 ms,
       then save module's run time and restart it
     */
    for (int i = 1; ; i++) {
        err = read_blob_size();
        ESP_LOGI(STORAGE_NAMESPACE, "Saving blob, pass %d", i);
        if (err != ESP_OK) ESP_LOGE(STORAGE_NAMESPACE, "Error (%s) reading data from NVS!", esp_err_to_name(err));
        err = save_blobs();
        if (err != ESP_OK) ESP_LOGW(STORAGE_NAMESPACE, "Error (%s) saving run time blob to NVS!", esp_err_to_name(err));
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    printf("Restarting...\n");
    fflush(stdout);
    esp_restart();
}
