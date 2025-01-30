package fuzz

import firrtl.options.{StageMain}
import chipyard.stage.ChipyardStage

// We are basically reusing everything from the chipyard generator.
// Taken from https://github.com/ucb-bar/chipyard/blob/main/generators/chipyard/src/main/scala/Generator.scala
object Generator extends StageMain(new ChipyardStage)
