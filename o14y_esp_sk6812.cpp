// (c) 2021 o14y
#include <string.h>
#include <memory>
#include <functional>
#include <driver/rmt.h>
#include <esp_err.h>
#include <esp_log.h>
#include <o14y_esp_sk6812.h>
#define UNPACK(p) ((o14y::esp::sk6812*)p)
#define ENSURE(ec) { esp_err_t e = (ec); if (e != ESP_OK){ return e; }}
const char* app = "o14y_esp_sk6812";
namespace o14y { namespace esp {
    class sk6812 {
    public:
        sk6812():config_(){}
        esp_err_t init(gpio_num_t gpio_num, rmt_channel_t channel) {
            ESP_LOGI("sk6812", "Configuring SK6812");
            rmt_config_t config = RMT_DEFAULT_CONFIG_TX(gpio_num, channel);
            memmove(&config_, &config, sizeof(config));
            config_.channel = channel;
            config_.clk_div = 8;  // for 80MHz. 24steps = 0.3usec for 1tick.
            ENSURE(rmt_config(&config_));
            ENSURE(rmt_driver_install(config_.channel, 0, 0));
            return ESP_OK;
        }
        esp_err_t set(int* g8r8b8, int count) {
            int nitems = count*24+1;
            std::shared_ptr<rmt_item32_t> items(new rmt_item32_t[nitems]
                , std::bind([](auto p){delete[] p;}, std::placeholders::_1));
            rmt_item32_t* itr=items.get();
            for ( int c=0; c<count; c++ ) {
                int color = g8r8b8[c];
                for ( int i=0; i<24; i++ ) {
                    itr->level0 = 1;
                    itr->level1 = 0;
                    if (color & 0x800000) {
                        // 1
                        itr->duration0 = 6;
                        itr->duration1 = 6;
                    } else {
                        // 0
                        itr->duration0 = 3;
                        itr->duration1 = 9;
                    }
                    color = color << 1;
                    itr++;
                }
            }
            itr->level0 = 0;
            itr->level1 = 0;
            itr->duration0 = 80;
            itr->duration1 = 0;
            ENSURE(rmt_write_items(config_.channel, items.get(), nitems, true));
            return ESP_OK;
        }
        ~sk6812(){
            esp_err_t ec = rmt_driver_uninstall(config_.channel);
            if (ec) {
                ESP_LOGE(app, "Failed to uninstall rmt driver %d\n", ec);
            }
        }
    private:
        rmt_config_t config_;
    };
} }
esp_err_t o14y_esp_sk6812_make(o14y_esp_sk6812_t* context, gpio_num_t gpio_num, rmt_channel_t channel) {
    o14y::esp::sk6812 *obj = new o14y::esp::sk6812();
    esp_err_t ec = obj->init(gpio_num, channel);
    if (ec == ESP_OK){
        *context = obj;
    }
    return ec;
}
esp_err_t o14y_esp_sk6812_clear(o14y_esp_sk6812_t context) {
    delete UNPACK(context);
    return ESP_OK;
}
esp_err_t o14y_esp_sk6812_set(o14y_esp_sk6812_t context, int* g8r8b8, int count) {
    o14y::esp::sk6812 *obj = UNPACK(context);
    return obj->set(g8r8b8, count);
}
esp_err_t o14y_esp_sk6812_setone(o14y_esp_sk6812_t context, int g8r8b8) {
    o14y::esp::sk6812 *obj = UNPACK(context);
    return obj->set(&g8r8b8, 1);
}