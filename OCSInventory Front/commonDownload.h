//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ModuleDownload.h: interface for the CModuleDownload class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COMMONDOWNLOAD_H_INCLUDED_
#define _COMMONDOWNLOAD_H_INCLUDED_

#pragma once

#include "FilePackageHistory.h"

// Download directory
#define OCS_DOWNLOAD_FOLDER		_T( "download")
// History of package installed
#define OCS_DOWNLOAD_HISTORY	_T( "history")
// Metadata info file
#define OCS_DOWNLOAD_METADATA	_T( "info")
// Agent create this file to disable Download processing this package
#define OCS_DOWNLOAD_SUSPEND	_T( "suspend")
// Create this file to lock package content
#define OCS_DOWNLOAD_LOCK		_T( "lock")
// Create this file and put timestamp inside, to allow checking for package timeout
#define OCS_DOWNLOAD_TIMESTAMP	_T( "since")
// This file contains list of package fragment to download
#define OCS_DOWNLOAD_TASK		_T( "task")
// This file contains package execution result
#define OCS_DOWNLOAD_DONE		_T( "done")
// This file contains OCSNG Windows Agent Setup execution result and
// is located under %ProgramData%\OCS Inventory NG\Agent\Download
#define OCS_AGENT_SETUP_DONE	_T( "OCSNG-Windows-Agent-Setup_done")
// This file contains package ID
#define OCS_DOWNLOAD_PACKAGE_ID	_T( "OCSNG-Windows-Agent-PackageID")
// Fragment files assembled create build.zip
#define OCS_DOWNLOAD_BUILD		_T( "build.zip")
// HKLM registry key to store package ID and digest
#define OCS_DOWNLOAD_REGISTRY	_T( "SOFTWARE\\OCS Inventory NG\\Agent\\Download")

// Download tool default configuration parameter
#define FRAG_LATENCY_DEFAULT	_T( "10")
#define PERIOD_LATENCY_DEFAULT	_T( "0")
#define CYCLE_LATENCY_DEFAULT	_T( "10")
#define MAX_ERROR_COUNT			30
#define PERIOD_LENGTH_DEFAULT	_T( "10")
#define TIMEOUT_DEFAULT			_T( "30")
#define COMMAND_TIMEOUT_DEFAULT	_T( "120")

// Download tool actions
#define OCS_DOWNLOAD_ACTION_LAUNCH	_T( "LAUNCH")
#define OCS_DOWNLOAD_ACTION_EXECUTE	_T( "EXECUTE")
#define OCS_DOWNLOAD_ACTION_STORE	_T( "STORE")

// Download tool error code
#define CODE_SUCCESS			_T( "SUCCESS")
#define ERR_BAD_ID				_T( "ERR_BAD_ID")
#define ERR_DOWNLOAD_INFO		_T( "ERR_DOWNLOAD_INFO")
#define ERR_BAD_DIGEST			_T( "ERR_BAD_DIGEST")
#define ERR_DOWNLOAD_PACK		_T( "ERR_DOWNLOAD_PACK")
#define ERR_BUILD				_T( "ERR_BUILD")
#define ERR_EXECUTE				_T( "ERR_EXECUTE")
#define ERR_CLEAN				_T( "ERR_CLEAN")
#define ERR_TIMEOUT				_T( "ERR_TIMEOUT")
#define ERR_ABORTED				_T( "ERR_ABORTED")
#define ERR_EXECUTE_PACK		_T( "ERR_EXECUTE_PACK")
#define ERR_ALREADY_SETUP		_T( "ERR_ALREADY_SETUP")
#define ERR_UNZIP				_T( "ERR_UNZIP")
#define ERR_NO_NAME				_T( "ERR_NO_PARAM")
#define ERR_NO_LAUNCHER			_T( "ERR_NO_LAUNCHER")
#define ERR_NO_INST32_CODE		_T( "ERR_NO_INST32_CODE")
#define ERR_BAD_PARAM			_T( "ERR_BAD_PARAM")
#define ERR_DONE_FAILED			_T( "ERR_DONE_FAILED")
#define ERR_OUT_OF_SPACE		_T( "ERR_OUT_OF_SPACE")
#define ERR_EXIT_CODE			_T( "EXIT_CODE_")

LPCTSTR OCSINVENTORYFRONT_API getPackageHistoryFilename();
LPCTSTR OCSINVENTORYFRONT_API getDownloadFolder();
// Used to suspend/resume download session by agent
// Create a suspend file to suspend download, and delete it to resume
BOOL OCSINVENTORYFRONT_API suspendDownload();
BOOL OCSINVENTORYFRONT_API resumeDownload();
// If suspend file exists, download is suspended
BOOL OCSINVENTORYFRONT_API isDownloadSuspended();
// Get exclusive acces to download directory by locking lock file
BOOL OCSINVENTORYFRONT_API lockDownload();
// Release exclusive access
BOOL OCSINVENTORYFRONT_API unlockDownload();

#endif // _COMMONDOWNLOAD_H_INCLUDED_
