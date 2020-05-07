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
#include <plugin_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <logger.h>
#include <plugin_exception.h>
#include <config_category.h>
#include <version.h>

typedef void (*INGEST_CB)(void *, Reading);

using namespace std;

/**
 *  Default configuration
 */
 const char *default_config = QUOTE({
     "plugin" : {
         "description" : "Simple J1939 plugin",
         "type" : "string",
         "default" : "J1939",
         "readonly" : "true"
     },
     "asset" : {
         "description" : "Asset name prefix for ingested readings data",
         "type" : "string",
         "default" : DEFAULT_ASSETNAME_PREFIX,
         "displayName" : "Asset Name prefix",
         "order" : "1"
     },
     "port" : {
         "description" : "Port of J1939 device",
         "type" : "string",
         "default" : "/dev/ttyUSB0",
         "displayName": "Port",
         "order" : "2"
     },
     "baud" : {
         "description" : "Baud rate of J1939",
         "type" : "integer",
         "default" : "57600",
         "displayName" : "Baud Rate",
         "order" : "3"
     },
     "bits" : {
         "description" : "Number of data bits for J1939",
         "type" : "integer",
         "default" : "8",
         "displayName" : "Number of Data Bits",
         "order" : "4"
     },
     "stopbits" : {
         "description" : "Number of stop bits for J1939",
         "type" : "integer",
         "default" : "1",
         "displayName" : "Number of Stop Bits",
         "order" : "5"
     },
     "parity" : {
         "description" : "Parity to use",
         "type" : "enumeration",
         "default" : "none",
         "options" : [ "none", "odd", "even" ],
         "displayName" : "Parity",
         "order" : "6"
     }
 });

/**
 * The J1939 plugin interface
 */
extern "C"{

/**
 * The plugin information structure
 */
static PLUGIN_INFORMATION info = {
    "J1939",                // Name
    VERSION,                // Version
    SP_ASYNC,               // Flags
    PLUGIN_TYPE_SOUTH,      // Type
    "1.0.0",                // Interface version
    default_config          // Default configuration
};

/**
 * Return the information about this plugin
 */
PLUGIN_INFORMATION *plugin_info()
{
    return &info;
}

PLUGIN_HANDLE plugin_init(ConfigCategory *config)
{
    J1939 *j1939 = new J1939();

        j1939->configure(config);

        return (PLUGIN_HANDLE)j1939;
}

/**
 * Start the Async handling for the plugin
 */
void plugin_start(PLUGIN_HANDLE *handle)
{
    J1939 *j1939 = (J1939 *)handle;

        j1939->start();
}

/**
 * Register ingest callback
 */
void plugin_register_ingest(PLUGIN_HANDLE *handle, INGEST_CB cb, void *data)
{
    J1939 *j1939 = (J1939 *)handle;

        if(!handle)
            throw new exception();
        j1939->registerIngest(data, cb);
}

/**
 * Poll for a plugin reading
 */
Reading plugin_poll(PLUGIN_HANDLE *handle)
{
    J1939 *j1939 = (J1939 *)handle;

        return j1939->nextValue();
}

/**
 * Reconfigure the plugin
 */
void plugin_reconfigure(PLUGIN_HANDLE *handle, string& newConfig)
{
    ConfigCategory config("J1939", newConfig);
    J1939           *j1939 = (J1939 *)handle;

        j1939->configure(&config);
}

/**
 *  Shutdown the plugin
 */
void plugin_shutdown(PLUGIN_HANDLE *handle)
{
    J1939 *j1939 = (J1939 *)handle;

        delete j1939;
}
};