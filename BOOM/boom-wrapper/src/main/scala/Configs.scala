package fuzz

import Chisel._
import boom.common._

import chipyard.harness._
import chipyard.iobinders._
import chipyard.config._

import freechips.rocketchip.subsystem._
import freechips.rocketchip.config._
import freechips.rocketchip.devices.debug._
import freechips.rocketchip.devices.tilelink._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.rocket._
import freechips.rocketchip.tile._
import freechips.rocketchip.util._

/*
 * Use our custom BootROM.
 */
 class WithFuzzBootROM extends Config((site, here, up) => {
  case BootROMLocated(x) => up(BootROMLocated(x), site)
      .map(_.copy(contentFileName = s"./generators/fuzz/boot/bootrom.rv64.img"))
})


/*
 * Add store buffer MDS following the example described in Fallout.
 */
class WithStoreBufferMDS extends Config((site, here, up) => {
  case TilesLocated(InSubsystem) => up(TilesLocated(InSubsystem), site) map {
    case tp: BoomTileAttachParams => tp.copy(tileParams = tp.tileParams.copy(core = tp.tileParams.core.copy(
      storeBufferMDS = true
    )))
    case other => other
  }
})

/*
 * Initialize all predictors to 0.
 */
class WithNoBIMInit extends Config((site, here, up) => {
  case TilesLocated(InSubsystem) => up(TilesLocated(InSubsystem), site) map {
    case tp: BoomTileAttachParams => tp.copy(tileParams = tp.tileParams.copy(core = tp.tileParams.core.copy(
      standardPredInit = true
    )))
    case other => other
  }
})

/*
 * Fine-grained parameters of the processor. Taken from rocketchip.
 */
class BaseSubsystemConfig extends Config ((site, here, up) => {
  // Tile parameters
  case PgLevels => if (site(XLen) == 64) 3 /* Sv39 */ else 2 /* Sv32 */
  case XLen => 64 // Applies to all cores
  case MaxHartIdBits => log2Up(site(TilesLocated(InSubsystem)).map(_.tileParams.hartId).max+1)
  // Interconnect parameters
  case SystemBusKey => SystemBusParams(
    beatBytes = site(XLen)/8,
    blockBytes = site(CacheBlockBytes))
  case ControlBusKey => PeripheryBusParams(
    beatBytes = site(XLen)/8,
    blockBytes = site(CacheBlockBytes),
    errorDevice = Some(BuiltInErrorDeviceParams(
      errorParams = DevNullParams(List(AddressSet(0x3000, 0xfff)), maxAtomic=site(XLen)/8, maxTransfer=4096))))
  case PeripheryBusKey => PeripheryBusParams(
    beatBytes = site(XLen)/8,
    blockBytes = site(CacheBlockBytes),
    dtsFrequency = Some(100000000)) // Default to 100 MHz pbus clock
  case MemoryBusKey => MemoryBusParams(
    beatBytes = site(XLen)/8,
    blockBytes = site(CacheBlockBytes))
  case FrontBusKey => FrontBusParams(
    beatBytes = site(XLen)/8,
    blockBytes = site(CacheBlockBytes))
  // Additional device Parameters
  case BootROMLocated(InSubsystem) => Some(BootROMParams(contentFileName = "./generators/fuzz/boot/bootrom.rv64.img"))
  case SubsystemExternalResetVectorKey => false
  case DebugModuleKey => Some(DefaultDebugModuleParams(site(XLen)))
  case CLINTKey => Some(CLINTParams())
  case PLICKey => Some(PLICParams())
  case TilesLocated(InSubsystem) =>
    LegacyTileFieldHelper(site(RocketTilesKey), site(RocketCrossingKey), RocketTileAttachParams.apply _)
})

/*
 * System-level configuration, adapted from BOOM.
 */
class BaseConfig extends Config(
  // The HarnessBinders control generation of hardware in the TestHarness
  new chipyard.harness.WithTiedOffDebug ++
  new chipyard.harness.WithBlackBoxSimMem ++                    // add SimDRAM DRAM model for axi4 backing memory, if axi4 mem is enabled
  new chipyard.harness.WithGPIOTiedOff ++                       // tie-off chiptop GPIOs, if GPIOs are present
  new chipyard.harness.WithTieOffInterrupts ++                  // tie-off interrupt ports, if present
  new chipyard.harness.WithTieOffL2FBusAXI ++                   // tie-off external AXI4 master, if present
  new chipyard.harness.WithClockAndResetFromHarness ++

  // The IOBinders instantiate ChipTop IOs to match desired digital IOs
  new chipyard.iobinders.WithAXI4MemPunchthrough ++
  new chipyard.iobinders.WithL2FBusAXI4Punchthrough ++
  new chipyard.iobinders.WithDividerOnlyClockGenerator ++

  // new chipyard.config.WithBootROM ++                                 // use default bootrom
  new chipyard.config.WithL2TLBs(1024) ++                           // use L2 TLBs
  new chipyard.config.WithNoSubsystemDrivenClocks ++                // drive the subsystem diplomatic clocks from ChipTop instead of using implicit clocks
  new chipyard.config.WithInheritBusFrequencyAssignments ++         // Unspecified clocks within a bus will receive the bus frequency if set
  new chipyard.config.WithPeripheryBusFrequencyAsDefault ++         // Unspecified frequencies with match the pbus frequency (which is always set)
  new chipyard.config.WithMemoryBusFrequency(100.0) ++              // Default 100 MHz mbus
  new chipyard.config.WithPeripheryBusFrequency(100.0) ++           // Default 100 MHz pbus
  new freechips.rocketchip.subsystem.WithInclusiveCache ++          // use Sifive L2 cache
  new freechips.rocketchip.subsystem.WithDontDriveBusClocksFromSBus ++ // leave the bus clocks undriven by sbus
  new freechips.rocketchip.subsystem.WithCoherentBusTopology ++      // hierarchical buses including sbus/mbus/pbus/fbus/cbus/l2
  new freechips.rocketchip.subsystem.WithDefaultMemPort ++           // enable AXI4 mem
  new freechips.rocketchip.subsystem.WithTimebase(BigInt(1000000)) ++ // 1 MHz
  new freechips.rocketchip.subsystem.WithDTS("freechips,rocketchip-unknown", Nil) ++
  new freechips.rocketchip.subsystem.WithNExtTopInterrupts(0) ++    // no external interrupts
  new fuzz.BaseSubsystemConfig
  )

// Available configurations.

class FuzzConfig extends Config(
    new fuzz.WithNoBIMInit ++
    new fuzz.WithFakeMem ++
    new fuzz.WithFuzzBootROM ++
    new boom.common.WithNMediumBooms(1) ++
    new fuzz.BaseConfig)

class StockConfig extends Config(
    new fuzz.WithFakeMem ++
    new fuzz.WithFuzzBootROM ++
    new boom.common.WithNMediumBooms(1) ++
    new fuzz.BaseConfig)

class MDSConfig extends Config(
    new fuzz.WithStoreBufferMDS ++
    new FuzzConfig)

class DebugConfig extends Config(
    new boom.common.WithBoomCommitLogPrintf ++
    // new boom.common.WithBoomBranchPrintf ++
    new FuzzConfig)

class SmallFuzzConfig extends Config(
    new fuzz.WithNoBIMInit ++
    new fuzz.WithFakeMem ++
    new fuzz.WithFuzzBootROM ++
    new boom.common.WithNSmallBooms(1) ++
    new fuzz.BaseConfig)
