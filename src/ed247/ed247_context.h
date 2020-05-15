/******************************************************************************
 * The MIT Licence
 *
 * Copyright (c) 2020 Airbus Operations S.A.S
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#ifndef _ED247_CONTEXT_H_
#define _ED247_CONTEXT_H_

#include "ed247_internals.h"
#include "ed247_xml.h"
#include "ed247_channel.h"

#include <vector>

namespace ed247
{

class SmartListFrames : public SmartList<std::shared_ptr<ed247_frame_t>>, public ed247_internal_frame_list_t
{
    public:
        using SmartList<std::shared_ptr<ed247_frame_t>>::SmartList;
        virtual ~SmartListFrames() {}
};

class SmartListActiveFrames : public SmartListFrames
{
    public:
        using SmartListFrames::SmartListFrames;
        virtual ~SmartListActiveFrames() {}

        virtual std::shared_ptr<ed247_frame_t> * next_ok() override
        {
                next_iter();
                _iter = std::find_if(_iter, std::vector<std::shared_ptr<ed247_frame_t>>::end(),
                    [](const std::shared_ptr<ed247_frame_t> & f){ return f->data != nullptr; });
                return current_value();
        }
};

class Context : public ed247_internal_context_t
{
    public:

        explicit Context(std::string ecic_filepath,
            const libed247_configuration_t & libed247_configuration = LIBED247_CONFIGURATION_DEFAULT);
        ~Context();
        Context(const Context &)                = delete;
        Context(Context &&)                     = delete;
        Context & operator = (const Context &)  = delete;
        Context & operator = (Context &&)       = delete;
        template<typename T>
        Context(std::initializer_list<T>)       = delete;

        void initialize();

        std::string getFilePath() const { return _ecic_filepath; }

        std::shared_ptr<xml::Root> getRoot() { return _root; }

        std::shared_ptr<BaseSignal::Pool> getPoolSignals() { return _pool_signals; }

        std::shared_ptr<BaseStream::Pool> getPoolStreams() { return _pool_streams; }

        std::shared_ptr<ComInterface::Pool> getPoolInterfaces() { return _pool_interfaces; }

        Channel::Pool * getPoolChannels() { return &_pool_channels; }

        void encode_frames()
        {
            encode(_root->info.identifier);
            _active_frames->reset();
            // Update frames
            for(auto & frame : *_active_frames){
                if(!frame->channel)
                    continue;
                auto channel = static_cast<Channel*>(frame->channel);
                if(!channel->buffer().empty()){
                    frame->data = channel->buffer().data();
                    frame->size = channel->buffer().size();
                }else{
                    frame->data = nullptr;
                    frame->size = 0;
                }
            }
        }

        void send_pushed_samples()
        {
            //encode(_root->info.identifier);
            //_pool_channels.send();
            _pool_channels.encode_and_send(_root->info.identifier);
        }
        ed247_status_t wait_frame(int32_t timeout_us)
        {
            _active_streams->reset();
            return _pool_interfaces->wait_frame(timeout_us);
        }
        ed247_status_t wait_during(int32_t duration_us)
        {
            _active_streams->reset();
            return _pool_interfaces->wait_during(duration_us);
        }

        std::shared_ptr<SmartListActiveFrames>    active_frames()  { return _active_frames;  }
        std::shared_ptr<SmartListActiveStreams>   active_streams() { return _active_streams; }

        const libed247_runtime_metrics_t* get_runtime_metrics();

        class Builder
        {
            public:
                static Context * create(std::string ecic_filepath,
                    const libed247_configuration_t & libed247_configuration = LIBED247_CONFIGURATION_DEFAULT);
                static void initialize(Context & context);

                Builder(const Builder &) = delete;
                Builder(Builder &&) = delete;
                Builder & operator = (const Builder &) = delete;
                Builder & operator = (Builder &&) = delete;

            private:
                Builder(){}

        };

    private:

        std::string                             _ecic_filepath;
        const libed247_configuration_t          _configuration;
        std::shared_ptr<xml::Root>              _root;
        std::shared_ptr<ComInterface::Pool>     _pool_interfaces;
        std::shared_ptr<BaseSignal::Pool>       _pool_signals;
        std::shared_ptr<BaseStream::Pool>       _pool_streams;
        Channel::Pool                           _pool_channels;
        std::shared_ptr<SmartListActiveFrames>  _active_frames;
        std::shared_ptr<SmartListActiveStreams> _active_streams;
        libed247_runtime_metrics_t              _runtime_metrics;

        void encode(ed247_uid_t component_identifier)
        {
            _pool_channels.encode(component_identifier);
        }
};

}

#endif