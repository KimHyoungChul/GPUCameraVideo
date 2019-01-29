package com.example.filterlibrary

import com.example.baselib.GCVInput
import com.example.baselib.GCVOutput

/**
 * Created by liuxuan on 2019/1/27
 */

class FilterGroup : GCVInput(), GCVOutput{

    var filterGroupAddress: Long = 0L

    init {
        filterGroupAddress = nativeFilterGroupInit()
        inputObjectAdress = filterGroupAddress
    }

    override fun nativeOutput(): Long {
        return filterGroupAddress
    }

    fun addFilter(filter: Filter){
        nativeFilterGroupAddFilter(filterGroupAddress, filter.getNativeFilterAdress())
    }

    fun removeFilter(filter: Filter){
        nativeFilterGroupRemoveFilter(filterGroupAddress, filter.getNativeFilterAdress())
    }

    private external fun nativeFilterGroupInit(): Long

    private external fun nativeFilterGroupAddFilter(filterGroupAddress: Long, nativeFilter: Long)

    private external fun nativeFilterGroupRemoveFilter(filterGroupAddress: Long, nativeFilter: Long)

}