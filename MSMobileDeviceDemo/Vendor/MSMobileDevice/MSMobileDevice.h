//
//  MSMobileDevice.h
//  MSMobileDeviceDemo
//
//  本源码由 Martin Lau 基于 GPL 开源
//  仅依赖系统库，不受限于Xcode 版本和iTunes 版本限制。
//
//  Created by MartinLau on 2017/3/19.
//  Copyright © 2017年 Martin Studio. All rights reserved.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     * its critical that we spell out all integer sizes, for the entry points
     * and data structures in the external DLL/framework that we have no
     * control over.
     */
#include <stdint.h>
    
#if defined(WIN32)
#include <CoreFoundation.h>
    typedef uint32_t kAMDError;
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <mach/error.h>
    
#import "MSMobileDeviceMacro.h"
#endif
    
    
    /* Error codes */
#define MDERR_APPLE_MOBILE		(err_system(0x3a))
#define MDERR_IPHONE			(err_sub(0))
    
    /* Apple Mobile (AM*) errors */
#define MDERR_OK                ERR_SUCCESS
#define MDERR_SYSCALL           (ERR_MOBILE_DEVICE | 0x01)
#define MDERR_OUT_OF_MEMORY     (ERR_MOBILE_DEVICE | 0x03)
#define MDERR_QUERY_FAILED      (ERR_MOBILE_DEVICE | 0x04)
#define MDERR_INVALID_ARGUMENT  (ERR_MOBILE_DEVICE | 0x0b)
#define MDERR_NO_SUCH_SERVICE   (ERR_MOBILE_DEVICE | 0x22)
#define MDERR_DICT_NOT_LOADED   (ERR_MOBILE_DEVICE | 0x25)
    
    /* Apple File Connection (AFC*) errors */
#define MDERR_AFC_OUT_OF_MEMORY 0x03
    
    /* Services, found in /System/Library/Lockdown/Services.plist */
    //com.apple.springboardservices
    //com.apple.mobile.installation_proxy
    //com.apple.mobile.house_arrest
    //com.apple.crashreportcopymover
    //com.apple.crashreportcopymobile
    //com.apple.misagent
    //com.apple.debug_image_mount
    //com.apple.mobile.integrity_relay
    //com.apple.mobile.MCInstall
    //com.apple.mobile.mobile_image_mounter
    //com.apple.mobile.diagnostics_relay
    //com.apple.mobile.factory_proxy
#define AMSVC_ATC                   "com.apple.atc"                                     //同步服务
#define AMSVC_AFC                   "com.apple.afc"
#define AMSVC_AFC2                  "com.apple.afc2"
#define AMSVC_BACKUP                "com.apple.mobilebackup"
#define AMSVC_CRASH_REPORT_COPY     "com.apple.crashreportcopy"
#define AMSVC_DEBUG_IMAGE_MOUNT     "com.apple.mobile.debug_image_mount"
#define AMSVC_NOTIFICATION_PROXY    "com.apple.mobile.notification_proxy"
#define AMSVC_PURPLE_TEST           "com.apple.purpletestr"
#define AMSVC_SOFTWARE_UPDATE       "com.apple.mobile.software_update"
#define AMSVC_SYNC                  "com.apple.mobilesync"
#define AMSVC_SCREENSHOT            "com.apple.screenshotr"
#define AMSVC_SYSLOG_RELAY          "com.apple.syslog_relay"
#define AMSVC_SYSTEM_PROFILER       "com.apple.mobile.system_profiler"
#define AMSVC_SCREENSHOTR           "com.apple.mobile.screenshotr"
#define AMSVC_FILE_RELAY            "com.apple.mobile.file_relay"
#define AMSVC_HOUSE_ARREST          "com.apple.mobile.house_arrest"                     //应用内文件服务
#define AMSVC_SPRINGBOARD_SERVICE   "com.apple.springboardservices"
#define AMSVC_INSTALLATION_PROXY    "com.apple.mobile.installation_proxy"
    
    
    
    // Apple's opaque types
    typedef uint32_t afc_error_t;
    typedef uint64_t afc_file_ref;
    
    typedef int am_service;
    typedef int am_connection;
    
    /* opaque structures */
    typedef struct _am_device				*am_device;
    typedef struct _am_restore_device		*am_restore_device;
    typedef struct _am_recovery_device		*am_recovery_device;
    
    
    typedef struct _afc_connection			*afc_connection;
    typedef struct _am_device_notification	*am_device_notification;
    typedef struct _afc_directory			*afc_directory;
    typedef struct _afc_dictionary			*afc_dictionary;
    typedef struct _afc_operation			*afc_operation;
    typedef void *                          ath_connection;
    
    typedef struct ssl_st                   *SSL;
    
    
    typedef union _kAMDUnionRef
    {
    long aaaa;
    char bbb[8];
    } kAMDUnionRef;
    
    /* Messages passed to device notification callbacks: passed as part of
     * am_device_notification_callback_info. */
    typedef enum {
        ADNCI_MSG_CONNECTED     = 1,
        ADNCI_MSG_DISCONNECTED  = 2,
        ADNCI_MSG_UNSUBSCRIBED  = 3
    } adnci_msg;
    
    struct am_device_notification_callback_info {
        am_device	dev;				/* 0    device */
        uint32_t	msg;				/* 4    one of adnci_msg */
    } __attribute__ ((packed));
    
    
    /* The type of the device notification callback function. */
    typedef void (*am_device_notification_callback) (struct am_device_notification_callback_info *, void* callback_data);
    
    /* The type of the device restore notification callback functions. */
    typedef void (*am_restore_device_notification_callback)(am_recovery_device *);
    
    
#pragma mark - Device
    
    /* ----------------------------------------------------------------------------
     *   Public routines
     * ------------------------------------------------------------------------- */
    
    /* Registers a notification with the current run loop. The callback gets
     * copied into the notification struct, as well as being registered with the
     * current run loop. callback_data gets passed to the callback in addition
     * to the info block.
     * unused0 and unused1 are both 0 when iTunes calls this.
     *
     *  Returns:
     *      MDERR_OK            if successful
     *      MDERR_SYSCALL       if CFRunLoopAddSource() failed
     *      MDERR_OUT_OF_MEMORY if we ran out of memory
     */
    kAMDError AMDeviceNotificationSubscribe(am_device_notification_callback callback,
                                            uint32_t unused0,
                                            uint32_t unused1,
                                            void *callback_data,
                                            am_device_notification *notification);
    
    /*
     * Unregisters notifications. Buggy (iTunes 8.2): if you subscribe, unsubscribe and subscribe again, arriving
     notifications will contain cookie and subscription from 1st call to subscribe, not the 2nd one. iTunes
     calls this function only once on exit.
     */
    kAMDError AMDeviceNotificationUnsubscribe(am_device_notification subscription);
    
    
    /* Registers for device notifications related to the restore process. unknown0
     * is zero when iTunes calls this. In iTunes,
     * the callbacks are located at:
     *      1: $3ac68e-$3ac6b1, calls $3ac542(unknown1, arg, 0)
     *      2: $3ac66a-$3ac68d, calls $3ac542(unknown1, 0, arg)
     *      3: $3ac762-$3ac785, calls $3ac6b2(unknown1, arg, 0)
     *      4: $3ac73e-$3ac761, calls $3ac6b2(unknown1, 0, arg)
     */
    kAMDError AMRestoreRegisterForDeviceNotifications(am_restore_device_notification_callback dfu_connect_callback, am_restore_device_notification_callback recovery_connect_callback, am_restore_device_notification_callback dfu_disconnect_callback, am_restore_device_notification_callback recovery_disconnect_callback, unsigned int unknown0, void *user_info);
    
    /*  Connects to the iPhone. Pass in the am_device structure that the
     *  notification callback will give to you.
     *
     *  Returns:
     *      MDERR_OK                if successfully connected
     *      MDERR_SYSCALL           if setsockopt() failed
     *      MDERR_QUERY_FAILED      if the daemon query failed
     *      MDERR_INVALID_ARGUMENT  if USBMuxConnectByPort returned 0xffffffff
     */
    kAMDError AMDeviceConnect(am_device device);
    
    kAMDError AMDeviceDisconnect(am_device device);
    kAMDError AMDeviceRetain(am_device device);
    kAMDError AMDeviceRelease(am_device device);
    
    // __DLLIMPORT kAMDError AMDevicePair(struct am_device device);
    kAMDError AMDevicePair(am_device device);
    kAMDError AMDeviceUnpair(am_device device);
    
    /*  Calls PairingRecordPath() on the given device, than tests whether the path
     *  which that function returns exists. During the initial connect, the path
     *  returned by that function is '/', and so this returns 1.
     *
     *  Returns:
     *      0   if the path did not exist
     *      1   if it did
     */
    int AMDeviceIsPaired(am_device device);
    
    
    /*  iTunes calls this function immediately after testing whether the device is
     *  paired. It creates a pairing file and establishes a Lockdown connection.
     *
     *  Returns:
     *      MDERR_OK                if successful
     *      MDERR_INVALID_ARGUMENT  if the supplied device is null
     *      MDERR_DICT_NOT_LOADED   if the load_dict() call failed
     */
    kAMDError AMDeviceValidatePairing(am_device device);
    
    
    /*  Creates a Lockdown session and adjusts the device structure appropriately
     *  to indicate that the session has been started. iTunes calls this function
     *  after validating pairing.
     *
     *  Returns:
     *      MDERR_OK                if successful
     *      MDERR_INVALID_ARGUMENT  if the Lockdown conn has not been established
     *      MDERR_DICT_NOT_LOADED   if the load_dict() call failed
     */
    kAMDError AMDeviceStartSession(am_device device);
    
    
    /* Starts a service and returns a handle that can be used in order to further
     * access the service. You should stop the session and disconnect before using
     * the service. iTunes calls this function after starting a session. It starts
     * the service and the SSL connection. unknown may safely be
     * NULL (it is when iTunes calls this), but if it is not, then it will be
     * filled upon function exit. service_name should be one of the AMSVC_*
     * constants. If the service is AFC (AMSVC_AFC), then the handle is the handle
     * that will be used for further AFC* calls.
     *
     * Returns:
     *      MDERR_OK                if successful
     *      MDERR_SYSCALL           if the setsockopt() call failed
     *      MDERR_INVALID_ARGUMENT  if the Lockdown conn has not been established
     */
    
    kAMDError AMDeviceStartService(am_device device,
                                   CFStringRef service_name,
                                   am_service *handle,
                                   uint32_t *unknown);
    
    kAMDError AMDeviceSecureStartService(am_device device,
                                         CFStringRef service_name,
                                         am_service *handle,
                                         am_connection *securitySocketCookie);
    
    int AMDServiceConnectionGetSocket(am_connection handle);
    
    /* Start and Stops a session. You should do this before accessing services.
     *
     * Returns:
     *      MDERR_OK                if successful
     *      MDERR_INVALID_ARGUMENT  if the Lockdown conn has not been established
     */
    kAMDError AMDeviceStartSession(am_device device);
    kAMDError AMDeviceStopSession(am_device device);
    
    uint32_t AMDeviceGetConnectionID(am_device device);
    uint32_t AMDeviceEnterRecovery(am_device device);
    
    uint32_t AMDeviceGetInterfaceType(am_device device);    // { return 1; //USB   return 2; //iTuens_WIFI }
    uint32_t AMDeviceGetInterfaceSpeed(am_device device);   // { return 0x78000; }           //废弃
    
    
    /* Reads various device settings; returns nil if no value is found for
     * the nominated key
     *
     * Must be balanced by CFRelease()
     *
     * Possible values for key:
     * ActivationState
     * ActivationStateAcknowledged
     * BasebandBootloaderVersion
     * BasebandVersion
     * BluetoothAddress
     * BuildVersion
     * DeviceCertificate
     * DeviceClass
     * DeviceName
     * DevicePublicKey
     * FirmwareVersion
     * HostAttached
     * IntegratedCircuitCardIdentity
     * InternationalMobileEquipmentIdentity
     * InternationalMobileSubscriberIdentity
     * ModelNumber
     * PhoneNumber
     * ProductType
     * ProductVersion
     * ProtocolVersion
     * RegionInfo
     * SBLockdownEverRegisteredKey
     * SIMStatus
     * SerialNumber
     * SomebodySetTimeZone
     * TimeIntervalSince1970
     * TimeZone
     * TimeZoneOffsetFromUTC
     * TrustedHostAttached
     * UniqueDeviceID
     * Uses24HourClock
     * WiFiAddress
     * iTunesHasConnectedAMDeviceSetWirelessBuddyFlags
     */
    CFTypeRef AMDeviceCopyValue(am_device device, CFStringRef domain, CFStringRef key);
    
    kAMDError AMDeviceSetValue(am_device device, CFStringRef domain, CFStringRef key, CFTypeRef value);
    kAMDError AMDeviceRemoveValue(am_device device, CFStringRef domain, CFStringRef key);
    
    /*
     * Returns the udid of the specified device.  The same value is returned
     * from AMDeviceCopyValue(device,0,"UniqueDeviceID").
     *
     * Must be balanced by CFRelease()
     */
    CFStringRef AMDeviceCopyDeviceIdentifier(am_device device);
    
    
    /*edits by geohot*/
    kAMDError AMDeviceDeactivate(am_device device);
    kAMDError AMDeviceActivate(am_device device, CFMutableDictionaryRef);
    /*end*/
    
    //    kAMDError AMDeviceProxyGetECID(am_device device);
    
    void AMDDeviceDetached(am_device device);
    
    SSL *AMDServiceConnectionGetSecureIOContext(am_connection conn);
    
#pragma mark - RecoveryDevice
    
    /* Causes the restore functions to spit out (unhelpful) progress messages to
     * the file specified by the given path. iTunes always calls this right before
     * restoring with a path of
     * "$HOME/Library/Logs/iPhone Updater Logs/iPhoneUpdater X.log", where X is an
     * unused number.
     */
    
    unsigned int AMRestoreEnableFileLogging(char *path);
    
    /* Initializes a new option dictionary to default values. Pass the constant
     * kCFAllocatorDefault as the allocator. The option dictionary looks as
     * follows:
     * {
     *      NORImageType => 'production',
     *      AutoBootDelay => 0,
     *      KernelCacheType => 'Release',
     *      UpdateBaseband => true,
     *      DFUFileType => 'RELEASE',
     *      SystemImageType => 'User',
     *      CreateFilesystemPartitions => true,
     *      FlashNOR => true,
     *      RestoreBootArgs => 'rd=md0 nand-enable-reformat=1 -progress'
     *      BootImageType => 'User'
     *  }
     *
     * Returns:
     *      the option dictionary   if successful
     *      NULL                    if out of memory
     */
    
    CFMutableDictionaryRef AMRestoreCreateDefaultOptions(CFAllocatorRef allocator);
    
    kAMDError AMRestorePerformRecoveryModeRestore(am_recovery_device *rdev, CFDictionaryRef opts, void *callback, void *user_info);
    kAMDError AMRestorePerformRestoreModeRestore(am_restore_device *rdev, CFDictionaryRef opts, void *callback, void *user_info);
    
    am_restore_device *AMRestoreModeDeviceCreate(unsigned int unknown0, unsigned int connection_id, unsigned int unknown1);
    
    kAMDError AMRestoreCreatePathsForBundle(CFStringRef restore_bundle_path, CFStringRef kernel_cache_type, CFStringRef boot_image_type, unsigned int unknown0, CFStringRef *firmware_dir_path, CFStringRef *kernelcache_restore_path, unsigned int unknown1, CFStringRef *ramdisk_path);
    
    kAMDError AMRecoveryModeDeviceSetAutoBoot(am_recovery_device *rdev, CFStringRef state, int unknown1, int unknown2, int unknown3);
    kAMDError AMRecoveryModeDeviceReboot(am_recovery_device *rdev, CFStringRef state, int unknown1, int unknown2, int unknown3);
    
#pragma mark - MessageOperate
    
    kern_return_t AMDeviceSendMessage(am_service socket, void *unused, CFPropertyListRef plist);
    kern_return_t AMDeviceReceiveMessage(am_service socket, CFDictionaryRef options, CFPropertyListRef * result);
    
#pragma mark - Application
    
    typedef void (*InstallationProxyCallBack)(CFDictionaryRef dict, void *arg);
    
    kAMDError AMDeviceLookupApplicationArchives(am_device device, CFDictionaryRef optionDict, CFDictionaryRef *archives);
    
    kAMDError AMDeviceLookupApplications(am_device device, CFDictionaryRef optionDict, CFDictionaryRef *appList);
    
    kAMDError AMDeviceRemoveApplicationArchive(am_service afcSock, CFStringRef cfBundleId, CFDictionaryRef optionDict, InstallationProxyCallBack callbackBlock, void *unknown);
    
    kAMDError AMDeviceUninstallApplication(am_service afcSock, CFStringRef cfBundleId, CFDictionaryRef optionDict, InstallationProxyCallBack callbackBlock, void *unknown);
    
    kAMDError AMDeviceInstallApplication(am_service installProxySock, CFStringRef cfStringPath, CFDictionaryRef optionDict, InstallationProxyCallBack installCallback, void *unknow);
    
    kAMDError AMDeviceSecureTransferPath(am_service afcSock, am_device device, CFStringRef cfStringPath, CFDictionaryRef optionDict, InstallationProxyCallBack transferCallback, void *delegate);
    
    kAMDError AMDeviceTransferApplication(am_service afcSock, CFStringRef cfStringPath, CFDictionaryRef optionDict, InstallationProxyCallBack transferCallback, void *unknown);
    
#pragma mark - Notification stuff
    
    // Notification stuff - only call these on "com.apple.mobile.notification_proxy" (AMSVC_NOTIFICATION_PROXY)
    typedef void (*NOTIFY_CALLBACK)(CFStringRef notification, void* data);
    
    kAMDError AMDPostNotification(am_service socket, CFStringRef notification, CFStringRef userinfo);
    
    kAMDError AMDObserveNotification(am_service socket, CFStringRef notification);
    
    kAMDError AMDListenForNotifications(am_service socket, NOTIFY_CALLBACK cb, void* data);
    
    kAMDError AMDShutdownNotificationProxy(am_service socket);
    
    
#pragma mark - AFC connection functions
    
    const char * AFCGetClientVersionString(void);		// "@(#)PROGRAM:afc  PROJECT:afc-80"
    
    afc_error_t AFCConnectionOpen(am_service handle,uint32_t io_timeout,afc_connection *conn);
    afc_error_t AFCConnectionClose(afc_connection conn);
    
    uint32_t AFCConnectionGetContext(afc_connection conn);
    uint32_t AFCConnectionSetContext(afc_connection conn, uint32_t ctx);
    
    uint32_t AFCConnectionGetContext(afc_connection conn);
    uint32_t AFCConnectionSetContext(afc_connection conn, uint32_t ctx);
    
    uint32_t AFCConnectionGetFSBlockSize(afc_connection conn);
    uint32_t AFCConnectionSetFSBlockSize(afc_connection conn, uint32_t size);
    
    uint32_t AFCConnectionGetIOTimeout(afc_connection conn);
    uint32_t AFCConnectionSetIOTimeout(afc_connection conn, uint32_t timeout);
    
    uint32_t AFCConnectionGetSocketBlockSize(afc_connection conn);
    uint32_t AFCConnectionSetSocketBlockSize(afc_connection conn, uint32_t size);
    
    kAMDError AFCConnectionSetSecureContext(afc_connection conn, SSL *context);
    
    CFStringRef AFCCopyErrorString(afc_connection a);
    CFTypeRef AFCConnectionCopyLastErrorInfo(afc_connection a);
    
    
    /* Opens an Apple File Connection. You must start the appropriate service
     * first with AMDeviceStartService(). In iTunes, io_timeout is 0.
     *
     * Returns:
     *      MDERR_OK                if successful
     *      MDERR_AFC_OUT_OF_MEMORY if malloc() failed
     */
    
    afc_error_t AFCDirectoryAccessOpen(am_service handle,
                                       uint32_t io_timeout,
                                       afc_connection *conn);
    
    /* Retrieves an afc_dictionary that describes the connected device.  To
     * extract values from the dictionary, use AFCKeyValueRead() and close
     * it when finished with AFCKeyValueClose()
     */
    afc_error_t AFCDeviceInfoOpen(afc_connection conn,
                                  afc_dictionary *info);
    
    /* Turns debug mode on if the environment variable AFCDEBUG is set to a numeric
     * value, or if the file '/AFCDEBUG' is present and contains a value. */
    void AFCPlatformInit();
    
    /* Opens a directory on the iPhone. Retrieves an afc_directory which can be
     * queried (with AFCDirectoryRead()) to enumerate through the filenames in
     * the directory.  Once finished, close with AFCDirectoryClose()
     *
     * Note that this normally only accesses the iTunes sandbox/partition as the
     * root, which is /var/root/Media. Pathnames are specified with '/' delimiters
     * as in Unix style.
     *
     * Returns:
     *      MDERR_OK                if successful
     */
    afc_error_t AFCDirectoryOpen(afc_connection conn,const char *path,afc_directory *dir);
    
    /* Acquires the next entry in a directory previously opened with
     * AFCDirectoryOpen(). When dirent is filled with a NULL value, then the end
     * of the directory has been reached. '.' and '..' will be returned as the
     * first two entries in each directory except the root; you may want to skip
     * over them.
     *
     * Returns:
     *      MDERR_OK                if successful, even if no entries remain
     */
    
    afc_error_t AFCDirectoryRead(afc_connection conn,
                                 afc_directory dir,
                                 char **dirent);
    
    /* Close the directory previously opened with AFCDirectoryOpen()
     */
    afc_error_t AFCDirectoryClose(afc_connection conn,
                                  afc_directory dir);
    
    /* Create a new directory on the device.
     */
    afc_error_t AFCDirectoryCreate(afc_connection conn, const char *dirname);
    
    /* Removes an existing file or directory from the device.
     */
    afc_error_t AFCRemovePath(afc_connection conn, const char *dirname);
    
    /* Renames an existing file or directory on the device */
    afc_error_t AFCRenamePath(afc_connection conn, const char *from, const char *to);
    
    /* Returns the context field of the given AFC connection. */
    uint32_t AFCDirectoryAccessGetContext(afc_connection conn);
    
    /* Set the context field of the given AFC connection.
     */
    uint32_t AFCDirectoryAccessSetContext(afc_connection conn, uint32_t ctx);
    
    /* Returns the fs_block_size field of the given AFC connection. */
    uint32_t AFCDirectoryAccessGetFSBlockSize(afc_connection conn);
    
    /* Returns the io_timeout field of the given AFC connection. In iTunes this is 0. */
    uint32_t AFCDirectoryAccessGetIOTimeout(afc_connection conn);
    
    uint32_t AFCDirectoryAccessSetIOTimeout(afc_connection conn, uint32_t timeout);
    
    /* Returns the sock_block_size field of the given AFC connection. */
    uint32_t AFCDirectoryAccessGetSocketBlockSize(afc_connection conn);
    
    /* Closes the given AFC connection. */
    afc_error_t AFCDirectoryAccessClose(afc_connection conn);
    
    /* ----------------------------------------------------------------------------
     *   Less-documented public routines
     * ------------------------------------------------------------------------- */
    afc_error_t AFCFileRefOpen(afc_connection conn,
                               const char *path,				/* pathname of file to open */
                               uint64_t mode,					/* 1=read, 2=write, 3=read/write */
                               afc_file_ref *ref);
    
    afc_error_t AFCFileRefSeek(afc_connection conn,
                               afc_file_ref ref,
                               int64_t offset,				/* signed offset from pos */
                               uint64_t pos);				/* 0=SEEK_SET, 1=SEEK_CUR, 2=SEEK_END */
    
    afc_error_t AFCFileRefTell(afc_connection conn,
                               afc_file_ref ref,
                               uint64_t *offset);
    
    // afc_error_t AFCFileRefLock(afc_connection *conn, afc_file_ref ref,
    //    ...);
    // int _AFCDirectoryAccessIsValid(afc_connection *conn)
    
    afc_error_t AFCFileRefRead(afc_connection conn, afc_file_ref ref, void *buf, uint32_t *len);
    
    afc_error_t AFCFileRefSetFileSize(afc_connection conn, afc_file_ref ref, uint64_t offset);
    
    afc_error_t AFCFileRefWrite(afc_connection conn, afc_file_ref ref, const void *buf, uint32_t len);
    
    afc_error_t AFCFileRefClose(afc_connection conn, afc_file_ref ref);
    
    afc_error_t AFCFileInfoOpen(afc_connection conn, const char *path, afc_dictionary *info);
    
    afc_error_t AFCKeyValueRead(afc_dictionary dict, char **key, char **val);
    
    afc_error_t AFCKeyValueClose(afc_dictionary dict);
    
    afc_error_t AFCLinkPath(afc_connection conn,uint64_t mode, const char *target,const char *link);
    
    afc_error_t AFCConnectionProcessOperation(afc_connection a1, afc_operation op, double timeout);
    afc_error_t AFCOperationGetResultStatus(afc_operation op);
    CFTypeRef AFCOperationGetResultObject(afc_operation op);
    CFTypeID AFCOperationGetTypeID(afc_operation op);
    afc_error_t AFCOperationSetContext(afc_operation op, void *ctx);
    void *AFCOperationGetContext(afc_operation op);
    
    // each of these returns an op, with the appropriate request encoded.  The value of ctx is
    // available via AFCOperationGetContext()
    afc_operation AFCOperationCreateGetConnectionInfo(CFAllocatorRef allocator, void *ctx);
    afc_operation AFCOperationCreateGetDeviceInfo(CFAllocatorRef allocator, void *ctx);
    afc_operation AFCOperationCreateGetFileHash(CFAllocatorRef allocator, CFStringRef filename, void *ctx);
    afc_operation AFCOperationCreateGetFileInfo(CFAllocatorRef allocator, CFStringRef filename, void *ctx);
    afc_operation AFCOperationCreateLinkPath(CFAllocatorRef allocator, uint32_t mode, CFStringRef filename1, CFStringRef filename2, void *ctx);
    afc_operation AFCOperationCreateMakeDirectory(CFAllocatorRef allocator, CFStringRef filename, void *ctx);
    afc_operation AFCOperationCreateOpenFile(CFAllocatorRef allocator, CFStringRef filename, void *ctx);
    afc_operation AFCOperationCreateReadDirectory(CFAllocatorRef allocator, CFStringRef filename, void *ctx);
    afc_operation AFCOperationCreateRemovePath(CFAllocatorRef allocator, CFStringRef filename, void *ctx);
    afc_operation AFCOperationCreateRenamePath(CFAllocatorRef allocator, CFStringRef oldname, CFStringRef newname, void *ctx);
    afc_operation AFCOperationCreateSetConnectionOptions(CFAllocatorRef allocator, CFDictionaryRef dict, void *ctx);
    afc_operation AFCOperationCreateSetModTime(CFAllocatorRef allocator, CFStringRef filename, uint64_t mtm, void *ctx);
    
    
#pragma mark - ATHost
    
    kAMDError ATHostConnectionSendSyncRequest(ath_connection ath, CFArrayRef dataclasses, CFDictionaryRef dataclassAnchors, CFDictionaryRef hostInfo);
    
    kAMDError ATHostConnectionSendMetadataSyncFinished(ath_connection ath, CFDictionaryRef keybag, CFDictionaryRef media);
    
    kAMDError ATHostConnectionSendFileError(ath_connection ath, CFStringRef pid, CFStringRef mediaType, int intType);
    
    kAMDError ATHostConnectionSendPowerAssertion(ath_connection ath, CFBooleanRef allocator);
    
    uint32_t ATHostConnectionSendHostInfo(ath_connection ath, CFDictionaryRef dictInfo);
    
    CFDictionaryRef ATCFMessageCreate(uint32_t sesssion, CFStringRef messageType, CFDictionaryRef params);
    
    uint32_t ATHostConnectionGetCurrentSessionNumber(ath_connection ath);
    
    uint32_t ATHostConnectionGetGrappaSessionId(ath_connection ath);
    
    uint32_t ATHostConnectionRelease(ath_connection ath);
    
    void ATProcessLinkSendMessage(ath_connection ath, CFDictionaryRef mATCFMessage);
    
    ath_connection ATHostConnectionCreateWithLibrary(CFStringRef prefsValue, CFStringRef identifier, uint32_t *unknown);
    
    CFTypeRef ATHostConnectionReadMessage(ath_connection ath);
    
#ifdef __cplusplus
}

#endif /* MSMobileDevice_h */
