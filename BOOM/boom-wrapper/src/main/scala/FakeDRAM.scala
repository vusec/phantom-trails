package fuzz

import chisel3._
import chisel3.util._
import chisel3.experimental.{IntParam, DataMirror, Direction}

import freechips.rocketchip.config.{Parameters}
import freechips.rocketchip.amba.axi4.{AXI4Bundle, AXI4BundleParameters}
import freechips.rocketchip.subsystem._

import testchipip._

import chipyard.{HasHarnessSignalReferences, HarnessClockInstantiatorKey}
import chipyard.iobinders.{GetSystemParameters, ClockWithFreq}
import chipyard.harness._



// Our fake DRAM, implemented as a "black-box" module (i.e. plain verilog).
// See resources/vsrc/DSimDRAM.v.
// Taken from https://github.com/ucb-bar/testchipip/blob/802d2b4a4d45556b00db1159f4af1b9d40eca8f6/src/main/scala/SimDRAM.scala.
class FakeDRAM(memSize: BigInt, lineSize: Int, clockFreqHz: BigInt,
              params: AXI4BundleParameters) extends BlackBox(Map(
    "MEM_SIZE" -> IntParam(memSize),
    "LINE_SIZE" -> IntParam(lineSize),
    "ADDR_BITS" -> IntParam(params.addrBits),
    "DATA_BITS" -> IntParam(params.dataBits),
    "ID_BITS" -> IntParam(params.idBits),
    "CLOCK_HZ" -> IntParam(clockFreqHz)
              )) with HasBlackBoxResource {

  val io = IO(new Bundle {
    val clock = Input(Clock())
    val reset = Input(Reset())
    val axi = Flipped(new AXI4Bundle(params))
  })

  require(params.dataBits <= 64)

  addResource("vsrc/FakeDRAM.v")
  addResource("csrc/dpi/FakeDRAM.cc")
}


// Mixin that adds our fake DRAM as an AXI device to the design.
// Extends the test harness class defined in the chipyard project.
// Modified from https://github.com/ucb-bar/chipyard/blob/main/generators/chipyard/src/main/scala/HarnessBinders.scala#L183
class WithFakeMem(additionalLatency: Int = 0) extends OverrideHarnessBinder({
  (system: CanHaveMasterAXI4MemPort, th: HasHarnessSignalReferences, ports: Seq[ClockedAndResetIO[AXI4Bundle]]) => {
    val p: Parameters = chipyard.iobinders.GetSystemParameters(system)
    (ports zip system.memAXI4Node.edges.in).map { case (port, edge) =>
      val memSize = p(ExtMem).get.master.size
      val lineSize = p(CacheBlockBytes)
      val clockFreq = p(MemoryBusKey).dtsFrequency.get
      val mem = Module(new FakeDRAM(memSize, lineSize, clockFreq, edge.bundle)).suggestName("simdram")
      mem.io.axi <> port.bits
      // Bug in Chisel implementation. See https://github.com/chipsalliance/chisel3/pull/1781
      def Decoupled[T <: Data](irr: IrrevocableIO[T]): DecoupledIO[T] = {
        require(DataMirror.directionOf(irr.bits) == Direction.Output, "Only safe to cast produced Irrevocable bits to Decoupled.")
        val d = Wire(new DecoupledIO(chiselTypeOf(irr.bits)))
        d.bits := irr.bits
        d.valid := irr.valid
        irr.ready := d.ready
        d
      }

      mem.io.clock := port.clock
      mem.io.reset := port.reset
    }
  }
})
