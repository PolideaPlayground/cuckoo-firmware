// MIT License

// Copyright (c) 2019 Polidea

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef LAP_TIMER_BLE_ATTRIBUTE_TABLE_H
#define LAP_TIMER_BLE_ATTRIBUTE_TABLE_H

#include "ble_types.h"
#include "ble_gatts.h"

///
/// @brief This is a class representing attribute table, which defines both
///        services and characteristics.
/// 
///
class AttributeTable {
    public:
        AttributeTable();
        AttributeTable(const AttributeTable&) = delete;
        AttributeTable(AttributeTable&&) = delete;
        AttributeTable& operator=(const AttributeTable&) = delete;
        AttributeTable& operator=(AttributeTable&&) = delete;

        ///
        /// @brief Initialize attribute table.
        /// @note This should be done after BLE initialization.
        ///
        void initialize();

        ///
        /// @brief Get the LapTimer service UUID.
        /// 
        /// @return ble_uuid_t LapTimer service UUID.
        ///
        ble_uuid_t get_service_uuid() const {
            return service_uuid;
        }

        ///
        /// @brief Get the LapTimer TX characteristic UUID.
        /// 
        /// This is TX from central device's perspecitive. This
        /// peripheral reads from this attribue.
        ///
        /// @return ble_uuid_t LapTimer TX characteristic.
        ///
        ble_uuid_t get_tx_characteristic_uuid() const {
            return tx_characteristic_uuid;
        }

        ///
        /// @brief Get the LapTimer RX characteristic UUID.
        /// 
        /// This is RX from central device's perspecitive. This
        /// peripheral reads from this attribue.
        ///
        /// @return ble_uuid_t LapTimer RX characteristic.
        ///
        ble_uuid_t get_rx_characteristic_uuid() const {
            return rx_characteristic_uuid;
        }

        ///
        /// @brief Get the LapTimer service handle.
        /// 
        /// @return uint16_t LapTimer service handle.
        ///
        uint16_t get_service_handle() const  {
            return service_handle;
        }

        ///
        /// @brief Get the LapTimer TX handle.
        /// 
        /// @return uint16_t LapTimer TX handle.
        ///
        uint16_t get_tx_characteristic_handle() const {
            return tx_characteristic_handles.value_handle;
        }

        ///
        /// @brief Get the LapTimer RX handle.
        /// 
        /// @return uint16_t LapTimer RX handle.
        ///
        uint16_t get_rx_characteristic_handle() const {
            return rx_characteristic_handles.value_handle;
        }

    private:
        ble_uuid128_t base_uuid128;
        uint8_t base_uuid_type;

        ble_uuid_t service_uuid;
        uint16_t service_handle;

        ble_uuid_t tx_characteristic_uuid;
        ble_gatts_char_md_t tx_characteristic_md;
        ble_gatts_attr_md_t tx_characteristic_attr_md;
        ble_gatts_attr_t tx_characteristic_attr;
        ble_gatts_char_handles_t tx_characteristic_handles;

        ble_uuid_t rx_characteristic_uuid;
        ble_gatts_char_md_t rx_characteristic_md;
        ble_gatts_attr_md_t rx_characteristic_attr_md;
        ble_gatts_attr_t rx_characteristic_attr;
        ble_gatts_char_handles_t rx_characteristic_handles;
};

#endif // LAP_TIMER_BLE_ATTRIBUTE_TABLE_H