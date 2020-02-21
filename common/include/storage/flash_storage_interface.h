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

#ifndef LAP_TIMER_FLASH_STORAGE_INTERFACE_H
#define LAP_TIMER_FLASH_STORAGE_INTERFACE_H

#include <cstdint>
#include <functional>

///
/// @brief Interface to the Flash Storage. 
/// @note This class is not thread safe and should be used only by one user.
///
class FlashStorageInterface {
public:
    ///
    /// @brief Delegate to be implemented by a user of this interface.
    /// 
    /// Make sure to execute all of the operations sequentially.
    ///
    class Delegate {
    public:
        ///
        /// @brief Flash Storage was initialized.
        /// 
        /// @param successful True if initialization was successful.
        /// @param interface Interface to the Flash Storage
        ///
        /// @note This may be called in interrupt context.
        ///
        virtual void on_initialized(bool successful, FlashStorageInterface& interface) = 0;

        ///
        /// @brief Flash Storage was garbage collected.
        /// 
        /// @param successful True if operation was successful.
        ///
        /// @note This may be called in interrupt context.
        ///
        virtual void on_garbage_collected(bool successful) = 0;

        ///
        /// @brief All files in the storage were removed.
        /// 
        /// @param successful True if operation was successful.
        ///
        /// @note This may be called in interrupt context.
        ///
        virtual void on_all_files_deleted(bool successful) = 0;

        ///
        /// @brief Selected file was removed.
        /// 
        /// @param successful True if operation was successful.
        /// @param file_id File ID of removed file.
        ///
        /// @note This may be called in interrupt context.
        ///
        virtual void on_file_deleted(bool successful, uint16_t file_id) = 0;

        ///
        /// @brief Record inside a file was removed.
        /// 
        /// @param successful True if operation was successful.
        /// @param file_id File ID of a file which contained a record.
        /// @param record_id Record ID of a removed record.
        ///
        /// @note This may be called in interrupt context.
        ///
        virtual void on_record_deleted(bool successful, uint16_t file_id, uint16_t record_id) = 0;

        ///
        /// @brief Record updated it's value.
        /// 
        /// @param successful True if operation was successful.
        /// @param file_id File ID of a file which contains a record.
        /// @param record_id Record ID of updated record.
        ///
        /// @note This may be called in interrupt context.
        ///
        virtual void on_record_written(bool successful, uint16_t file_id, uint16_t record_id) = 0;
    };

public:
    ///
    /// @brief Set the delegate object
    /// 
    /// @param delegate 
    ///
    virtual void set_delegate(Delegate *delegate) = 0;

    ///
    /// @brief Collect garbage. By default removed records and files are not cleared up. It's up to the user
    ///        to select proper time to do that.
    /// @note It's asynchronous operation.
    /// 
    /// @return true Request was successfully submitted. Wait for on_garbage_collected callback.
    /// @return false Coudn't execute the request. Please retry after some time.
    ///
    virtual bool collect_garbage() = 0;

    ///
    /// @brief Delete all files on the storage. Other operations are not allowed until this procedure finishes.
    /// @note It's asynchronous operation.
    /// 
    /// @return true Request was successfully submitted. Wait for on_all_files_deleted callback.
    /// @return false Coudn't execute the request. Please retry after some time.
    ///
    virtual bool delete_all_files() = 0;

    ///
    /// @brief Delete a file with all of its records.
    /// @note It's asynchronous operation.
    /// 
    /// @param file_id File ID of a file to be deleted.
    /// @return true Request was successfully submitted. Wait for on_file_deleted callback.
    /// @return false Coudn't execute the request. Please retry after some time.
    ///
    virtual bool delete_file(uint16_t file_id) = 0;

    ///
    /// @brief Delete a record inside a specific file.
    /// @note It's asynchronous operation.
    /// 
    /// @param file_id File ID of a file containing the record.
    /// @param record_id Record ID of a record to be removed.
    /// @return true Request was successfully submitted. Wait for on_record_deleted callback.
    /// @return false Coudn't execute the request. Please retry after some time.
    ///
    virtual bool delete_record(uint16_t file_id, uint16_t record_id) = 0;

    ///
    /// @brief Read a specific record.
    /// @note It's synchronous operation.
    /// 
    /// @param file_id File ID of a file containing a record.
    /// @param record_id Record ID of a record to be read.
    /// @param data Pointer to the data, which will store record's contents. Note it's 4 byte aligned data.
    /// @param words_count Number of words (4 bytes) to be read. Value can be updated if record contains smaller number of words.
    /// @return true Request was successfully executed.
    /// @return false Coudn't execute the request, probably due to invalid parameters.
    ///
    virtual bool read_record(uint16_t file_id, uint16_t record_id, uint32_t* data, uint16_t *words_count) = 0;

    ///
    /// @brief Write to a specific record.
    /// @note It's asynchronous operation.
    ///
    /// @param file_id File ID of a file containing a record.
    /// @param record_id Record ID of a record to be written.
    /// @param data Data to be written to the record. Note it's 4 byte aligned data. This pointer must be valid until callback is received.
    /// @param words_count Number of words (4 bytes) to be written.
    /// @return true Request was successfully submitted. Wait for on_record_written callback.
    /// @return false Coudn't execute the request. Please retry after some time.
    ///
    virtual bool write_record(uint16_t file_id, uint16_t record_id, const uint32_t* data, uint16_t words_count) = 0;

    ///
    /// @brief Iterates over all records in all files.
    ///
    /// Order in which records are passed as parameters is undetermined.
    ///
    /// @note It's synchronous operation.
    /// @param callback Callback, which returns file_id, record_id, record_data and record_data length.
    /// @return true Request was successfully executed.
    /// @return false Coudn't execute the request.
    ///
    virtual bool iterate_records(std::function<void(uint16_t file_id, uint16_t record_id, const uint32_t* record_data, uint16_t record_data_length)> callback) = 0;
};

#endif // LAP_TIMER_FLASH_STORAGE_INTERFACE_H