//
//  kern_rad.hpp
//  WhateverGreen
//
//  Copyright © 2017 vit9696. All rights reserved.
//

#ifndef kern_rad_hpp
#define kern_rad_hpp

#include <Headers/kern_patcher.hpp>
#include <Headers/kern_devinfo.hpp>
#include <Library/LegacyIOService.h>
#include "kern_atom.hpp"
#include "kern_con.hpp"

class RAD {
public:
	void init();
	void deinit();

	/**
	 *  AMD Hardware kext index
	 */
	enum HardwareIndex {
		IndexRadeonHardwareX4000,
		IndexRadeonHardwareX5000,
		IndexRadeonHardwareX3000,
		IndexRadeonHardwareX4100,
		IndexRadeonHardwareX4150,
		IndexRadeonHardwareX4200,
		IndexRadeonHardwareX4250,
		MaxRadeonHardware,
		MaxRadeonHardwareMojave = IndexRadeonHardwareX5000 + 1,
		MaxRadeonHardwareModernHighSierra = IndexRadeonHardwareX3000 + 1
	};

	/**
	 *  Property patching routine
	 *
	 *  @param patcher  KernelPatcher instance
	 *  @param info     device info
	 */
	void processKernel(KernelPatcher &patcher, DeviceInfo *info);

	/**
	 *  Patch kext if needed and prepare other patches
	 *
	 *  @param patcher KernelPatcher instance
	 *  @param index   kinfo handle
	 *  @param address kinfo load address
	 *  @param size    kinfo memory size
	 *
	 *  @return true if patched anything
	 */
	bool processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);

private:
	/**
	 *  Private self instance for callbacks
	 */
	static RAD *callbackRAD;

	/**
	 *  Original set property function
	 */
	mach_vm_address_t orgSetProperty {};

	/**
	 *  Original get property function
	 */
	mach_vm_address_t orgGetProperty {};

	/**
	 *  Original connector info functions
	 */
	mach_vm_address_t orgGetConnectorsInfoV1 {};
	mach_vm_address_t orgGetConnectorsInfoV2 {};
	mach_vm_address_t orgLegacyGetConnectorsInfo {};

	/**
	 *  Get atom object table function type
	 */
	using t_getAtomObjectTableForType = void *(*)(void *that, AtomObjectTableType type, uint8_t *sz);

	/**
	 *  Original get atom object table functions
	 */
	t_getAtomObjectTableForType orgGetAtomObjectTableForType {nullptr};
	t_getAtomObjectTableForType orgLegacyGetAtomObjectTableForType {nullptr};

	/**
	 *  Original translate atom connector into modern connector function
	 */
	mach_vm_address_t orgTranslateAtomConnectorInfoV1 {};
	mach_vm_address_t orgTranslateAtomConnectorInfoV2 {};

	/**
	 *  Original controller start functions
	 */
	mach_vm_address_t orgATIControllerStart {};
	mach_vm_address_t orgLegacyATIControllerStart {};

	/**
	 *  Current controller property provider
	 */
	IOService *currentPropProvider {nullptr};
	IOService *currentLegacyPropProvider {nullptr};

	/**
	 *  Original populateAccelConfig functions
	 */
	mach_vm_address_t orgPopulateAccelConfig[MaxRadeonHardware] {};

	/**
	 *  Max framebuffer base functions per kext
	 */
	static constexpr size_t MaxGetFrameBufferProcs = 3;

	/**
	 *  Framebuffer base function names
	 */
	const char *getFrameBufferProcNames[MaxRadeonHardware][MaxGetFrameBufferProcs] {
		[IndexRadeonHardwareX3000] = {
			"__ZN15AMDR8xxHardware25getFrameBufferBaseAddressEv"
		},
		[IndexRadeonHardwareX4000] = {
			"__ZN13AMDSIHardware25getFrameBufferBaseAddressEv",
			"__ZN13AMDCIHardware25getFrameBufferBaseAddressEv",
			"__ZN28AMDRadeonX4000_AMDVIHardware25getFrameBufferBaseAddressEv"
		},
		[IndexRadeonHardwareX4100] = {
			"__ZN28AMDRadeonX4100_AMDVIHardware25getFrameBufferBaseAddressEv"
		},
		[IndexRadeonHardwareX4150] = {
			"__ZN28AMDRadeonX4150_AMDVIHardware25getFrameBufferBaseAddressEv"
		},
		[IndexRadeonHardwareX4200] = {
			"__ZN28AMDRadeonX4200_AMDVIHardware25getFrameBufferBaseAddressEv"
		},
		[IndexRadeonHardwareX4250] = {
			"__ZN28AMDRadeonX4250_AMDVIHardware25getFrameBufferBaseAddressEv"
		},
	};

	/**
	 *  populateAccelConfig function type
	 */
	using t_populateAccelConfig = void (*)(IOService *accelService, const char **accelConfig);

	/**
	 *  Wrapped populateAccelConfig functions
	 */
	t_populateAccelConfig wrapPopulateAccelConfig[MaxRadeonHardware] {
		[RAD::IndexRadeonHardwareX3000] = populdateAccelConfig<RAD::IndexRadeonHardwareX3000>,
		[RAD::IndexRadeonHardwareX4100] = populdateAccelConfig<RAD::IndexRadeonHardwareX4000>,
		[RAD::IndexRadeonHardwareX4150] = populdateAccelConfig<RAD::IndexRadeonHardwareX4100>,
		[RAD::IndexRadeonHardwareX4200] = populdateAccelConfig<RAD::IndexRadeonHardwareX4150>,
		[RAD::IndexRadeonHardwareX4250] = populdateAccelConfig<RAD::IndexRadeonHardwareX4200>,
		[RAD::IndexRadeonHardwareX4000] = populdateAccelConfig<RAD::IndexRadeonHardwareX4250>,
		[RAD::IndexRadeonHardwareX5000] = populdateAccelConfig<RAD::IndexRadeonHardwareX5000>
	};

	/**
	 *  Register read function names
	 */
	const char *populateAccelConfigProcNames[MaxRadeonHardware] {
		[RAD::IndexRadeonHardwareX3000] = "__ZN37AMDRadeonX3000_AMDGraphicsAccelerator19populateAccelConfigEP13IOAccelConfig",
		[RAD::IndexRadeonHardwareX4100] = "__ZN37AMDRadeonX4000_AMDGraphicsAccelerator19populateAccelConfigEP13IOAccelConfig",
		[RAD::IndexRadeonHardwareX4150] = "__ZN37AMDRadeonX4100_AMDGraphicsAccelerator19populateAccelConfigEP13IOAccelConfig",
		[RAD::IndexRadeonHardwareX4200] = "__ZN37AMDRadeonX4150_AMDGraphicsAccelerator19populateAccelConfigEP13IOAccelConfig",
		[RAD::IndexRadeonHardwareX4250] = "__ZN37AMDRadeonX4200_AMDGraphicsAccelerator19populateAccelConfigEP13IOAccelConfig",
		[RAD::IndexRadeonHardwareX4000] = "__ZN37AMDRadeonX4250_AMDGraphicsAccelerator19populateAccelConfigEP13IOAccelConfig",
		[RAD::IndexRadeonHardwareX5000] = "__ZN37AMDRadeonX5000_AMDGraphicsAccelerator19populateAccelConfigEP13IOAccelConfig"
	};

	/**
	 *  Limit DVI resolution to single link
	 */
	bool dviSingleLink {false};

	/**
	 *  Disable Metal support (and force OpenGL)
	 */
	bool forceOpenGL {false};

	/**
	 *  Report real GPU name in Accelerator config
	 */
	bool fixConfigName {false};

	/**
	 *  Boot ATI/AMD graphics without acceleration
	 */
	bool forceVesaMode {false};

	/**
	 *  Current max used hardware kexts
	 */
	size_t maxHardwareKexts {MaxRadeonHardware};

	/**
	 *  Configure available kexts for different OS
	 */
	void initHardwareKextMods();

	/**
	 *  Merge configuration properties from ioreg
	 *
	 *  @param props     target dictionary with original properties
	 *  @param prefix    property name prefix in provider
	 *  @param provider  property provider for merging
	 */
	void mergeProperties(OSDictionary *props, const char *prefix, IOService *provider);

	/**
	 *  Automatically add properties to fix various bugs
	 *
	 *  @param service       gpu controller
	 *  @param connectorNum  number of connectors
	 */
	void applyPropertyFixes(IOService *service, uint32_t connectorNum=0);

	/**
	 *  Refresh connectors with the provided ones if necessary
	 *
	 *  @param atomutils  AtiAtomBiosUtilities instance
	 *  @param gettable   relevant atom object table getting function
	 *  @param ctrl       ATIController service
	 *  @param connectors autodetected controllers
	 *  @param sz         number of autodetected controllers
	 */
	void updateConnectorsInfo(void *atomutils, t_getAtomObjectTableForType gettable, IOService *ctrl, RADConnectors::Connector *connectors, uint8_t *sz);

	/**
	 *  Apply various fixes to automatically detected connectors
	 *
	 *  @param baseAddr            atom object offset base address
	 *  @param displayPaths        pointer to atom display object path table
	 *  @param connectorObjects    pointer to connector objects table
	 *  @param connectorObjectNum  number of elements in connectorObjects
	 *  @param connectors          pointer to autodetected connectors
	 *  @param sz                  number of autodetected connectors
	 */
	void autocorrectConnectors(uint8_t *baseAddr, AtomDisplayObjectPath *displayPaths, uint8_t displayPathNum, AtomConnectorObject *connectorObjects,
							   uint8_t connectorObjectNum, RADConnectors::Connector *connectors, uint8_t sz);

	/**
	 *  Actually correct a certain found connector
	 *
	 *  @param connector   connector id
	 *  @param sense       sense id
	 *  @param txmit       transmitter
	 *  @param enc         encoder
	 *  @param connectors  pointer to autodetected connectors
	 *  @param sz          number of autodetected connectors
	 */
	void autocorrectConnector(uint8_t connector, uint8_t sense, uint8_t txmit, uint8_t enc, RADConnectors::Connector *connectors, uint8_t sz);

	/**
	 *  Changes connector priority according to provided sense id list
	 *
	 *  @param senseList   list of sense ids in ascending order
	 *  @param senseNum    number of sense ids in the list
	 *  @param connectors  pointer to autodetected connectors
	 *  @param sz          number of autodetected connectors
	 */
	void reprioritiseConnectors(const uint8_t *senseList, uint8_t senseNum, RADConnectors::Connector *connectors, uint8_t sz);

	/**
	 *  populateAccelConfig wrapping functions used for accelerator name correction
	 */
	template <size_t Index>
	static void populdateAccelConfig(IOService *accelService, const char **accelConfig) {
		if (callbackRAD->orgPopulateAccelConfig[Index]) {
			FunctionCast(populdateAccelConfig<Index>, callbackRAD->orgPopulateAccelConfig[Index])(accelService, accelConfig);
			callbackRAD->updateAccelConfig(accelService, accelConfig);
		} else {
			SYSLOG("rad", "populdateAccelConfig invalid use for %lu", Index);
		}
	}

	/**
	 *  Enable forced 24-bit output
	 *
	 *  @param patcher  kernel patcher instance
	 *  @param info     loaded kinfo of the right framebuffer
	 *  @param address  kinfo load address
	 *  @param size     kinfo memory size
	 */
	void process24BitOutput(KernelPatcher &patcher, KernelPatcher::KextInfo &info, mach_vm_address_t address, size_t size);

	/**
	 *  Apply connector modifications (for support kexts)
	 *
	 *  @param patcher  kernel patcher instance
	 *  @param address  kinfo load address
	 *  @param size     kinfo memory size
	 *  @param modern   legacy or normal kext
	 */
	void processConnectorOverrides(KernelPatcher &patcher, mach_vm_address_t address, size_t size, bool modern);

	/**
	 *  Apply hardware kext modifications (X3000~X5000)
	 *
	 *  @param patcher  kernel patcher instance
	 *  @param hwIndex  hardware kext index
	 *  @param address  kinfo load address
	 *  @param size     kinfo memory size
	 */
	void processHardwareKext(KernelPatcher &patcher, size_t hwIndex, mach_vm_address_t address, size_t size);

	/**
	 *  Update IOAccelConfig with a real GPU model name
	 *
	 *  @param accelService IOAccelerator service
	 *  @param accelConfig  IOAccelConfig
	 */
	void updateAccelConfig(IOService *accelService, const char **accelConfig);

	/**
	 *  Wrapped set property function
	 */
	static bool wrapSetProperty(IORegistryEntry *that, const char *aKey, void *bytes, unsigned length);
	
	/**
	 *  Wrapped get property function
	 */
	static OSObject *wrapGetProperty(IORegistryEntry *that, const char *aKey);
	
	/**
	 *  Wrapped get connectors info functions
	 */
	static uint32_t wrapGetConnectorsInfoV1(void *that, RADConnectors::Connector *connectors, uint8_t *sz);
	static uint32_t wrapGetConnectorsInfoV2(void *that, RADConnectors::Connector *connectors, uint8_t *sz);
	static uint32_t wrapLegacyGetConnectorsInfo(void *that, RADConnectors::Connector *connectors, uint8_t *sz);
	
	/**
	 *  Wrapped translate atom connector into modern connector functions
	 */
	static uint32_t wrapTranslateAtomConnectorInfoV1(void *that, RADConnectors::AtomConnectorInfo *info, RADConnectors::Connector *connector);
	static uint32_t wrapTranslateAtomConnectorInfoV2(void *that, RADConnectors::AtomConnectorInfo *info, RADConnectors::Connector *connector);
	
	/**
	 *  Wrapped ATIController start functions
	 */
	static bool wrapATIControllerStart(IOService *ctrl, IOService *provider);
	static bool wrapLegacyATIControllerStart(IOService *ctrl, IOService *provider);
};

#endif /* kern_rad_hpp */
