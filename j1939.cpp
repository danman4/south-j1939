/*
 * FogLAMP J1939 south plugin.
 *
 * Copyright (c) 2020 Nex Computers
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Daniel Man
 */

#include <j1939.h>
#include <config_category.h>
#include <reading.h>
#include <stdio.h>
#include <stdlib.h>
#include <logger.h>
#include <unistd.h>

using namespace std;

static void threadWrapper(J1939 *j1939)
{
    j1939->pollThread();
}

/**
 * Constructor for the J1939 plugin
 */
J1939::J1939()
{
}

/**
 * Destructor for the J1939 plugin
 */
J1939::~J1939()
{
    m_running = false;
    m_thread->join();
}

void J1939::start()
{
    m_running = true;
    m_thread = new thread(threadWrapper, this);
}

void J1939::pollThread()
{
    while(m_running)
    {
        (*m_ingest)(m_data, nextValue());
        sleep(1);
    }
}

void J1939::configure(ConfigCategory *config)
{
    setAssetName(config->getValue("asset"));
    setPort(config->getValue("port"));
    int baud = 57600;
    char parity = 'N';
    int bits = 8;
    int stopBits = 1;
    if (config->itemExists("baud"))
    {
        string value = config->getValue("baud");
        baud = atoi(value.c_str());
    }
    if  (config->itemExists("parity"))
    {
        string value = config->getValue("parity");
        if (value.compare("even") == 0)
        {
            parity = 'E';
        }
        else if (value.compare("odd") == 0)
        {
            parity = 'O';
        }
        else if (value.compare("none") == 0)
        {
            parity = 'N';
        }
    }
    if  (config->itemExists("bits"))
    {
        string value = config->getValue("bits");
        bits = atoi(value.c_str());
    }
    if  (config->itemExists("stopBits"))
    {
        string value = config->getValue("stopBits");
        stopBits = atoi(value.c_str());
    }
    if  (m_baud != baud)
    {
        m_baud = baud;
    }
    if  (m_parity != parity)
    {
        m_parity = parity;
    }
    if  (m_bits != bits)
    {
        m_bits = bits;
    }
    if  (m_stopBits = stopBits)
    {
        m_stopBits = stopBits;
    }

    m_fp = fopen(m_port.c_str(), "r");
    if (m_fp == NULL)
    {
        throw runtime_error("Unable to open file");
    }
}

/**
 * Take a reading from the J1939 plugin
 */
Reading J1939::nextValue()
{
    char    buffer[80], *ptr;
    int     ch;

        ptr = buffer;
        while((ch = fgetc(m_fp)) != EOF && ! (ch == '\r' || ch == '\n') && ptr - buffer < sizeof(buffer))
        {
            *ptr++ = ch;
        }
        *ptr = 0;
        if (ch == EOF)
        {
            fseek(m_fp, 0L, SEEK_SET);
        }
        DatapointValue value(buffer);
        return Reading(m_asset, new Datapoint(m_asset, value));
}