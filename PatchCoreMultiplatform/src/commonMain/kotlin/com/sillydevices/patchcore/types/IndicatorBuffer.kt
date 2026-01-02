package com.sillydevices.patchcore.types

interface IndicatorBuffer {

    fun getWriteIndex(): Int
    fun getList(): List<Float>
}