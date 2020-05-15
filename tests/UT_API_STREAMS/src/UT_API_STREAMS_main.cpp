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

/************
 * Includes *
 ************/

#include <ed247_test.h>

#include <stdio.h>
#include <regex>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

/***********
 * Defines *
 ***********/

/******************************************************************************
This test file checks the functions that manipulate channels are working correctly
******************************************************************************/
 
class UtApiStreams : public ::testing::Test{};

/******************************************************************************
This first test case checks loading of the streams is correctly performed.
The basic getter for streams is partially validated as well as stream list
manipulation function.
******************************************************************************/	
TEST(UtApiStreams, LoadStreams)
{
    ed247_context_t context;
    ed247_stream_list_t stream_list;
    ed247_stream_t stream;
    const ed247_stream_info_t* stream_info;
    
    const char* ecic_filename = CONFIG_PATH"/ut_api_streams/ecic_single_channel.xml";
    ASSERT_EQ(ed247_load(ecic_filename, NULL, &context), ED247_STATUS_SUCCESS);
    
    // Get the stream list using the getter, check the unvalid call configurations
    
    // First tests validate the parsing of the ecic file
    ASSERT_EQ(ed247_component_get_streams(NULL, &stream_list), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_component_get_streams(context, NULL), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_component_get_streams(context, &stream_list), ED247_STATUS_SUCCESS);

    // Check stream list size
    size_t size;
    ASSERT_EQ(ed247_stream_list_size(stream_list, &size), ED247_STATUS_SUCCESS);
    ASSERT_EQ(size, (size_t)16);
    
    // Check the retrieved content, perform unvalid calls to verify robustness
    ASSERT_EQ(ed247_stream_list_next(stream_list, NULL), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_stream_list_next(NULL, &stream), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    
    // Get the stream info and check the values, perform unvalid calls to verify robustness
    ASSERT_EQ(ed247_stream_get_info(NULL, &stream_info), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_stream_get_info(NULL, &stream_info), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_stream_get_info(stream, NULL), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream2");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_OUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_A429);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->comment, "") == 0);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->icd, "") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)1);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)1);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)4);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream2full"));
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_IN);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_A429);
    ASSERT_TRUE(strcmp(stream_info->comment, "Optional for A429") == 0);
    ASSERT_TRUE(strcmp(stream_info->icd, "ICD for A429") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)101);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)3);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)4);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream3"));
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_INOUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_A825);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->comment, "") == 0);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->icd, "") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)2);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)1);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)69);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream3full"));
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_INOUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_A825);
    ASSERT_TRUE(strcmp(stream_info->comment, "A825 designates CAN") == 0);
    ASSERT_TRUE(strcmp(stream_info->icd, "ICD for A825") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)102);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)17);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)69);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream1");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_OUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_A664);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->comment, "") == 0);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->icd, "") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)0);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)1);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)1471);
    
    // Perform checks for all other Streams
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream1full");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_IN);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_A664);
    ASSERT_TRUE(strcmp(stream_info->comment, "Optional for A664") == 0);
    ASSERT_TRUE(strcmp(stream_info->icd, "Test for A664") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)100);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)42);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)1471);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "StreamSerialFull"));
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_INOUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_SERIAL);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->comment, "SERIAL line") == 0);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->icd, "ICD for SERIAL") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)1002);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)27);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)123);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "StreamSerial"));
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_OUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_SERIAL);
    ASSERT_TRUE(strcmp(stream_info->comment, "") == 0);
    ASSERT_TRUE(strcmp(stream_info->icd, "") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)1003);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)1);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)321);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream4");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_OUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_DISCRETE);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->comment, "") == 0);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->icd, "") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)3);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)1);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)4);
    ASSERT_EQ(stream_info->info.dis.sampling_period_us, (uint32_t)10000);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream4full");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_IN);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_DISCRETE);
    ASSERT_TRUE(strcmp(stream_info->comment, "Discrete input signals") == 0);
    ASSERT_TRUE(strcmp(stream_info->icd, "ICD for DSI") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)103);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)14);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)4);
    ASSERT_EQ(stream_info->info.dis.sampling_period_us, (uint32_t)15000);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream5");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_OUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_ANALOG);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->comment, "") == 0);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->icd, "") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)4);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)1);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)16);
    ASSERT_EQ(stream_info->info.ana.sampling_period_us, (uint32_t)20000);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream5full");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_IN);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_ANALOG);
    ASSERT_TRUE(strcmp(stream_info->comment, "Analog input signals") == 0);
    ASSERT_TRUE(strcmp(stream_info->icd, "ICD for ANA") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)104);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)36);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)16);
    ASSERT_EQ(stream_info->info.ana.sampling_period_us, (uint32_t)25000);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream6");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_IN);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_NAD);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->comment, "") == 0);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->icd, "") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)5);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)1);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)6);
    ASSERT_EQ(stream_info->info.nad.sampling_period_us, (uint32_t)100000);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream6full");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_OUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_NAD);
    ASSERT_TRUE(strcmp(stream_info->comment, "Non Avionic Data input signals") == 0);
    ASSERT_TRUE(strcmp(stream_info->icd, "ICD for NAD") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)105);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)23);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)15);
    ASSERT_EQ(stream_info->info.nad.sampling_period_us, (uint32_t)110000);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream7");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_IN);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_VNAD);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->comment, "") == 0);
    ASSERT_TRUE(stream_info->comment != NULL && strcmp(stream_info->icd, "") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)6);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)1);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)12);
    ASSERT_EQ(stream_info->info.vnad.sampling_period_us, (uint32_t)500000);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream7full");
    ASSERT_EQ(stream_info->direction, ED247_DIRECTION_OUT);
    ASSERT_EQ(stream_info->type, ED247_STREAM_TYPE_VNAD);
    ASSERT_TRUE(strcmp(stream_info->comment, "Variable Non Avionic Data input signals") == 0);
    ASSERT_TRUE(strcmp(stream_info->icd, "ICD for VNAD") == 0);
    ASSERT_EQ(stream_info->uid, (ed247_uid_t)106);
    ASSERT_EQ(stream_info->sample_max_number, (size_t)12);
    ASSERT_EQ(stream_info->sample_max_size_bytes, (size_t)13);
    ASSERT_EQ(stream_info->info.vnad.sampling_period_us, (uint32_t)1000000);
    
    // Check the end of the list is reached and that on next request it will restart from the beginning
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(stream, (ed247_stream_t)NULL);
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream2");
    
    // Remove this list
    ASSERT_EQ(ed247_stream_list_free(NULL), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_stream_list_free(stream_list), ED247_STATUS_SUCCESS);
    
    ASSERT_EQ(ed247_unload(context), ED247_STATUS_SUCCESS);
}

TEST(UtApiStreams, CheckFindStreamMethod)
{
    ed247_context_t context;
    ed247_channel_list_t channel_list;
    ed247_channel_t channel, channel_test;
    ed247_stream_list_t stream_list;
    ed247_stream_t stream;
    const ed247_stream_info_t* stream_info;
    
    const char* ecic_filename = CONFIG_PATH"/ut_api_streams/ecic_multiple_channels.xml";
    ASSERT_EQ(ed247_load(ecic_filename, NULL, &context), ED247_STATUS_SUCCESS);

    ASSERT_EQ(ed247_find_channels(context, "MultipleStreamsChannel", &channel_list), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_channel_list_next(channel_list, &channel), ED247_STATUS_SUCCESS);

    ASSERT_EQ(ed247_find_streams(NULL, NULL, &stream_list), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_find_streams(context, NULL, NULL), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_find_streams(context, ".*[", &stream_list), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_find_streams(context, NULL, &stream_list), ED247_STATUS_SUCCESS);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream2"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream3"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream1"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream8"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream4"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream5"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream6"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream7"));

    ASSERT_EQ(ed247_stream_get_channel(stream, &channel_test), ED247_STATUS_SUCCESS);
    ASSERT_EQ(channel, channel_test);

    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream12"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream13"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream11"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream18"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream14"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream15"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream16"));
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream17"));

    // Check the end of the list is reached and that on next request it will restart from the beginning
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(stream, (ed247_stream_t)NULL);
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(stream_info->name, "Stream2"));
    
    // Remove this list
    ASSERT_EQ(ed247_stream_list_free(stream_list), ED247_STATUS_SUCCESS);
    
    ASSERT_EQ(ed247_unload(context), ED247_STATUS_SUCCESS);
}


TEST(UtApiStreams, CheckGetStreamFromContext)
{
    ed247_context_t context;
    ed247_channel_list_t channel_list;
    ed247_channel_t channel;
    const ed247_channel_info_t* channel_info;
    ed247_stream_list_t stream_list;
    ed247_stream_t stream;
    const ed247_stream_info_t* stream_info;

    const char* ecic_filename = CONFIG_PATH"/ut_api_streams/ecic_multiple_channels.xml";
    ASSERT_EQ(ed247_load(ecic_filename, NULL, &context), ED247_STATUS_SUCCESS);

    // Get the first channel and retrieve streams
    ASSERT_EQ(ed247_find_channels(context, "MultipleStreamsChannel", &channel_list), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_channel_list_next(channel_list, &channel), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_channel_get_info(channel, &channel_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(channel_info->name, "MultipleStreamsChannel"));
    ASSERT_EQ(ed247_channel_get_streams(NULL, &stream_list), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_channel_get_streams(channel, NULL), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_channel_get_streams(channel, &stream_list), ED247_STATUS_SUCCESS);
    
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream1");
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream2");
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream3");
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream4");
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream5");
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream6");
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream7");
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream8");
    
    // Check the end of the list is reached and that on next request it will restart from the beginning
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(stream, (ed247_stream_t)NULL);
    ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
    ASSERT_STREQ(stream_info->name, "Stream1");
    
    // Remove this list
    ASSERT_EQ(ed247_stream_list_free(stream_list), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_channel_list_free(channel_list), ED247_STATUS_SUCCESS);
    
    ASSERT_EQ(ed247_unload(context), ED247_STATUS_SUCCESS);
}

TEST(UtApiStreams, CheckRegexStreamFromContext)
{
    ed247_context_t context;
    ed247_channel_list_t channel_list;
    ed247_channel_t channel;
    const ed247_channel_info_t* channel_info;
    ed247_stream_list_t stream_list;
    ed247_stream_t stream;
    const ed247_stream_info_t* stream_info;

    const char* ecic_filename = CONFIG_PATH"/ut_api_streams/ecic_multiple_channels.xml";
    ASSERT_EQ(ed247_load(ecic_filename, NULL, &context), ED247_STATUS_SUCCESS);

    // Check channel list size
    ASSERT_EQ(ed247_find_channels(context, ".*", &channel_list), ED247_STATUS_SUCCESS);
    size_t size;
    ASSERT_EQ(ed247_channel_list_size(channel_list, &size), ED247_STATUS_SUCCESS);
    ASSERT_EQ(size, (size_t)2);

    // Get a single channel and retrieve streams
    ASSERT_EQ(ed247_find_channels(context, "MultipleStreamsChannel2", &channel_list), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_channel_list_next(channel_list, &channel), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_channel_get_info(channel, &channel_info), ED247_STATUS_SUCCESS);
    ASSERT_FALSE(strcmp(channel_info->name, "MultipleStreamsChannel2"));
    ASSERT_EQ(ed247_find_channel_streams(NULL, NULL, &stream_list), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_find_channel_streams(channel, NULL, NULL), ED247_STATUS_FAILURE);
    ASSERT_EQ(ed247_find_channel_streams(channel, ".*[", &stream_list), ED247_STATUS_FAILURE);
    
    if(atoi("This case cannot be unrolled because gcc does not supports regex before version 4.9.X") != 0)
    {
        ASSERT_EQ(ed247_find_channel_streams(channel, "Stream1[1357]", &stream_list), ED247_STATUS_SUCCESS);
        ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
        ASSERT_TRUE(ed247_stream_get_info(stream, &stream_info) == ED247_STATUS_SUCCESS);
        ASSERT_TRUE(stream_info->name != NULL && strcmp(stream_info->name, "Stream11") == 0);
        
        ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
        ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
        ASSERT_TRUE(stream_info != NULL && stream_info->name != NULL && strcmp(stream_info->name, "Stream13") == 0);

        ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
        ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
        ASSERT_TRUE(stream_info != NULL && stream_info->name != NULL && strcmp(stream_info->name, "Stream15") == 0);

        ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
        ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
        ASSERT_TRUE(stream_info != NULL && stream_info->name != NULL && strcmp(stream_info->name, "Stream17") == 0);
        
        // Check the end of the list is reached and that on next request it will restart from the beginning
        ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
        ASSERT_EQ(stream, (ed247_stream_t)NULL);
        ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
        ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
        ASSERT_TRUE(stream_info->name != NULL && strcmp(stream_info->name, "Stream11") == 0);
    }
    else
    {
        ASSERT_EQ(ed247_find_channel_streams(channel, "Stream11", &stream_list), ED247_STATUS_SUCCESS);
        ASSERT_EQ(ed247_stream_list_next(stream_list, &stream), ED247_STATUS_SUCCESS);
		ASSERT_EQ(ed247_stream_get_info(stream, &stream_info), ED247_STATUS_SUCCESS);
		ASSERT_NE(stream_info, (const ed247_stream_info_t*)NULL);
		ASSERT_NE(stream_info->name, (const char*)NULL);
        ASSERT_EQ(strcmp(stream_info->name, "Stream11"), 0);

    }

    // Remove this list
    ASSERT_EQ(ed247_stream_list_free(stream_list), ED247_STATUS_SUCCESS);
    ASSERT_EQ(ed247_channel_list_free(channel_list), ED247_STATUS_SUCCESS);
    
    ASSERT_EQ(ed247_unload(context), ED247_STATUS_SUCCESS);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}