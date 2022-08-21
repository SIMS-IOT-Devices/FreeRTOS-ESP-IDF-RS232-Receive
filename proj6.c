// Receive one byte via RS232 ("A" = 01000001 = 65 and "A" + parity bit = 010000010)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"

static const int RX_BUF_SIZE = 1024;

#define UART_0_TX 1
#define UART_0_RX 3

void init()
{
    const uart_port_t uart_num = UART_NUM_0;
    const int uart_buffer_size = (RX_BUF_SIZE * 2);
    QueueHandle_t uart_queue;
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE, // 0
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, UART_0_TX, UART_0_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0);
}

static void rx_task()
{
    const uart_port_t uart_num = UART_NUM_0;
    int length = 0;
    int buff_length = 0;
    uint8_t data[128];

    while (1)
    {
        buff_length = uart_get_buffered_data_len(uart_num, (size_t *)&length);
        length = uart_read_bytes(uart_num, data, length, 100);
        printf("data - %d\n", data[0]);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
void app_main()
{
    printf("Receive data:\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    init();
    xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
}