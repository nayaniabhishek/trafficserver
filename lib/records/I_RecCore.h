/** @file

  Public RecCore declarations

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#ifndef _I_REC_CORE_H_
#define _I_REC_CORE_H_

#include "Diags.h"

#include "I_RecDefs.h"
#include "I_RecAlarms.h"
#include "I_RecSignals.h"
#include "I_RecEvents.h"


//-------------------------------------------------------------------------
// Diagnostic Output
//-------------------------------------------------------------------------
int RecSetDiags(Diags * diags);

//-------------------------------------------------------------------------
// Config File Parsing
//-------------------------------------------------------------------------
typedef void (*RecConfigEntryCallback)(RecT rec_type, RecDataT data_type, const char * name, const char * value, bool inc_version);

void RecConfigFileInit(void);
int RecConfigFileParse(const char * path, RecConfigEntryCallback handler, bool inc_version);

// Return a copy of the system's local state directory, taking proxy.config.local_state_dir into account. The
// caller MUST release the result with ats_free().
char * RecConfigReadRuntimeDir();

// Return a copy of the system's snapshot directory, taking proxy.config.snapshot_dir into account. The caller
// MUST release the result with ats_free().
char * RecConfigReadSnapshotDir();

// Return a copy of the system's log directory, taking proxy.config.log.logfile_dir into account. The caller
// MUST release the result with ats_free().
char * RecConfigReadLogDir();

// Return a copy of a configuration file that is relative to sysconfdir. The relative path to the configuration
// file is specified in the configuration variable named by "file_variable". If the configuration variable has no
// value, NULL is returned. The caller MUST release the result with ats_free().
char * RecConfigReadConfigPath(const char * file_variable, const char * default_value = NULL);

// Return a copy of the persistent stats file. This is $RUNTIMEDIR/records.snap.
// The caller MUST release the result with ats_free().
char * RecConfigReadPersistentStatsPath();

// Test whether the named configuration value is overridden by an environment variable. Return either
// the overridden value, or the original value. Caller MUST NOT free the result.
const char * RecConfigOverrideFromEnvironment(const char * name, const char * value);

//-------------------------------------------------------------------------
// Stat Registration
//-------------------------------------------------------------------------
int _RecRegisterStatInt(RecT rec_type, const char *name, RecInt data_default, RecPersistT persist_type);
#define RecRegisterStatInt(rec_type, name, data_default, persist_type) _RecRegisterStatInt((rec_type), (name), (data_default), REC_PERSISTENCE_TYPE(persist_type))

int _RecRegisterStatFloat(RecT rec_type, const char *name, RecFloat data_default, RecPersistT persist_type);
#define RecRegisterStatFloat(rec_type, name, data_default, persist_type) _RecRegisterStatFloat((rec_type), (name), (data_default), REC_PERSISTENCE_TYPE(persist_type))

int _RecRegisterStatString(RecT rec_type, const char *name, RecString data_default, RecPersistT persist_type);
#define RecRegisterStatString(rec_type, name, data_default, persist_type) _RecRegisterStatString((rec_type), (name), (data_default), REC_PERSISTENCE_TYPE(persist_type))

int _RecRegisterStatCounter(RecT rec_type, const char *name, RecCounter data_default, RecPersistT persist_type);
#define RecRegisterStatCounter(rec_type, name, data_default, persist_type) _RecRegisterStatCounter((rec_type), (name), (data_default), REC_PERSISTENCE_TYPE(persist_type))

//-------------------------------------------------------------------------
// Config Registration
//-------------------------------------------------------------------------

int RecRegisterConfigInt(RecT rec_type, const char *name,
                         RecInt data_default, RecUpdateT update_type,
                         RecCheckT check_type, const char *ccheck_regex, RecAccessT access_type = RECA_NULL);

int RecRegisterConfigFloat(RecT rec_type, const char *name,
                           RecFloat data_default, RecUpdateT update_type,
                           RecCheckT check_type, const char *check_regex, RecAccessT access_type = RECA_NULL);

int RecRegisterConfigString(RecT rec_type, const char *name,
                            const char *data_default, RecUpdateT update_type,
                            RecCheckT check_type, const char *check_regex, RecAccessT access_type = RECA_NULL);

int RecRegisterConfigCounter(RecT rec_type, const char *name,
                             RecCounter data_default, RecUpdateT update_type,
                             RecCheckT check_type, const char *check_regex, RecAccessT access_type = RECA_NULL);

//-------------------------------------------------------------------------
// Config Change Notification
//-------------------------------------------------------------------------

int RecLinkConfigInt(const char *name, RecInt * rec_int);
int RecLinkConfigInt32(const char *name, int32_t * p_int32);
int RecLinkConfigUInt32(const char *name, uint32_t * p_uint32);
int RecLinkConfigFloat(const char *name, RecFloat * rec_float);
int RecLinkConfigCounter(const char *name, RecCounter * rec_counter);
int RecLinkConfigString(const char *name, RecString * rec_string);
int RecLinkConfigByte(const char *name, RecByte * rec_byte);

int RecRegisterConfigUpdateCb(const char *name, RecConfigUpdateCb update_cb, void *cookie);
int RecRegisterRawStatUpdateFunc(const char *name, RecRawStatBlock * rsb, int id, RecStatUpdateFunc update_func, void *cookie);


//-------------------------------------------------------------------------
// Record Reading/Writing
//-------------------------------------------------------------------------

// WARNING!  Avoid deadlocks by calling the following set/get calls
// with the appropiate locking conventions.  If you're calling these
// functions from a configuration update callback (RecConfigUpdateCb),
// be sure to set 'lock' to 'false' as the hash-table rwlock has
// already been taken out for the callback.

// RecSetRecordConvert -> WebMgmtUtils.cc::varSetFromStr()
int RecSetRecordConvert(const char *name, const RecString rec_string, bool lock = true, bool inc_version = true);
int RecSetRecordInt(const char *name, RecInt rec_int, bool lock = true, bool inc_version = true);
int RecSetRecordFloat(const char *name, RecFloat rec_float, bool lock = true, bool inc_version = true);
int RecSetRecordString(const char *name, const RecString rec_string, bool lock = true, bool inc_version = true);
int RecSetRecordCounter(const char *name, RecCounter rec_counter, bool lock = true, bool inc_version = true);

int RecGetRecordInt(const char *name, RecInt * rec_int, bool lock = true);
int RecGetRecordFloat(const char *name, RecFloat * rec_float, bool lock = true);
int RecGetRecordString(const char *name, char *buf, int buf_len, bool lock = true);
int RecGetRecordString_Xmalloc(const char *name, RecString * rec_string, bool lock = true);
int RecGetRecordCounter(const char *name, RecCounter * rec_counter, bool lock = true);
// Convenience to allow us to treat the RecInt as a single byte internally
int RecGetRecordByte(const char *name, RecByte * rec_byte, bool lock = true);

//------------------------------------------------------------------------
// Record Attributes Reading
//------------------------------------------------------------------------
int RecGetRecordType(const char *name, RecT * rec_type, bool lock = true);
int RecGetRecordDataType(const char *name, RecDataT * data_type, bool lock = true);
int RecGetRecordPersistenceType(const char *name, RecPersistT * persist_type, bool lock = true);
int RecGetRecordOrderAndId(const char *name, int *order, int *id, bool lock = true);

int RecGetRecordUpdateType(const char *name, RecUpdateT * update_type, bool lock = true);
int RecGetRecordCheckType(const char *name, RecCheckT * check_type, bool lock = true);
int RecGetRecordCheckExpr(const char *name, char **check_expr, bool lock = true);
int RecGetRecordDefaultDataString_Xmalloc(char *name, char **buf, bool lock = true);

int RecGetRecordAccessType(const char *name, RecAccessT * secure, bool lock = true);
int RecSetRecordAccessType(const char *name, RecAccessT secure, bool lock = true);

void RecGetRecordTree(char *subtree = NULL);
void RecGetRecordList(char *, char ***, int *);
int RecGetRecordPrefix_Xmalloc(char *prefix, char **result, int *result_len);


//------------------------------------------------------------------------
// Signal and Alarms
//------------------------------------------------------------------------
void RecSignalManager(int, const char *);


//-------------------------------------------------------------------------
// Backwards Compatibility Items (REC_ prefix)
//-------------------------------------------------------------------------
#define REC_ReadConfigInt32(_var,_config_var_name) do { \
  RecInt tmp = 0; \
  RecGetRecordInt(_config_var_name, (RecInt*) &tmp); \
  _var = (int32_t)tmp; \
} while (0)

#define REC_ReadConfigInteger(_var,_config_var_name) do { \
  RecInt tmp = 0; \
  RecGetRecordInt(_config_var_name, &tmp); \
  _var = tmp; \
} while (0)

#define REC_ReadConfigFloat(_var,_config_var_name) do { \
  RecFloat tmp = 0; \
  RecGetRecordFloat(_config_var_name, &tmp); \
  _var = tmp; \
} while (0)

#define REC_ReadConfigStringAlloc(_var,_config_var_name) \
  RecGetRecordString_Xmalloc(_config_var_name, (RecString*)&_var)

#define REC_ReadConfigString(_var, _config_var_name, _len) \
  RecGetRecordString(_config_var_name, _var, _len)

#define REC_RegisterConfigUpdateFunc(_config_var_name, func, flag) \
  RecRegisterConfigUpdateCb(_config_var_name, func, flag)

#define REC_EstablishStaticConfigInteger(_var, _config_var_name) do { \
  RecLinkConfigInt(_config_var_name, &_var); \
  _var = (int64_t)REC_ConfigReadInteger(_config_var_name); \
} while (0)

#define REC_EstablishStaticConfigInt32(_var, _config_var_name) do { \
  RecLinkConfigInt32(_config_var_name, &_var); \
  _var = (int32_t)REC_ConfigReadInteger(_config_var_name); \
} while (0)

#define REC_EstablishStaticConfigInt32U(_var, _config_var_name) do { \
  RecLinkConfigUInt32(_config_var_name, &_var); \
  _var = (int32_t)REC_ConfigReadInteger(_config_var_name); \
} while (0)

/*
 * RecLinkConfigString allocates the RecString and stores the ptr to it (&var).
 * So before changing _var (the RecString) we have to free the original one.
 * Really, we somehow need to know whether RecLinkConfigString allocated _var.
 * For now, we're using the return value to indicate this, even though it's
 * not always the case.  If we're wrong, we'll leak the RecString.
 */
#define REC_EstablishStaticConfigStringAlloc(_var, _config_var_name) do { \
  if (RecLinkConfigString(_config_var_name, &_var) == REC_ERR_OKAY) \
    ats_free(_var);                                                    \
  _var = (RecString)REC_ConfigReadString(_config_var_name); \
} while (0)

#define REC_EstablishStaticConfigFloat(_var, _config_var_name) do { \
  RecLinkConfigFloat(_config_var_name, &_var); \
  _var = (RecFloat)REC_ConfigReadFloat(_config_var_name); \
} while (0)

// Allow to treat our "INT" configs as a byte type internally. Note
// that the byte type is just a wrapper around RECD_INT.
#define REC_EstablishStaticConfigByte(_var, _config_var_name) do { \
    RecLinkConfigByte(_config_var_name, &_var); \
    _var = (RecByte)REC_ConfigReadInteger(_config_var_name);    \
  } while (0)

RecInt REC_ConfigReadInteger(const char *name);
char *REC_ConfigReadString(const char *name);
RecFloat REC_ConfigReadFloat(const char *name);
RecCounter REC_ConfigReadCounter(const char *name);

// MGMT2 Marco's -- converting lmgmt->record_data->readXXX
RecInt REC_readInteger(const char *name, bool * found, bool lock = true);
RecFloat REC_readFloat(char *name, bool * found, bool lock = true);
RecCounter REC_readCounter(char *name, bool * found, bool lock = true);
RecString REC_readString(const char *name, bool * found, bool lock = true);

//------------------------------------------------------------------------
// Clear Statistics
//------------------------------------------------------------------------
int RecResetStatRecord(const char *name);
int RecResetStatRecord(RecT type = RECT_NULL, bool all = false);


//------------------------------------------------------------------------
// Set RecRecord attributes
//------------------------------------------------------------------------
int RecSetSyncRequired(char *name, bool lock = true);


//------------------------------------------------------------------------
// Signal Alarm/Warning/Error
//------------------------------------------------------------------------
#define REC_SignalManager        RecSignalManager
#define REC_SignalWarning(_n,_d) { Warning("%s", _d); RecSignalManager(_n,_d); }
#define REC_SignalError(_buf, _already)  {                                \
  if(_already == false)                                                   \
	REC_SignalManager(REC_SIGNAL_CONFIG_ERROR, _buf);                 \
  _already = true;                                                        \
  Warning("%s", _buf);                                                    \
}


//------------------------------------------------------------------------
// Manager Callback
//------------------------------------------------------------------------
typedef void *(*RecManagerCb) (void *opaque_cb_data, char *data_raw, int data_len);
int RecRegisterManagerCb(int _signal, RecManagerCb _fn, void *_data = NULL);

void RecResizeAdditional(int add);

#endif
