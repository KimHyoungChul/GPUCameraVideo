//
// Created by 刘轩 on 2019/1/27.
//

#include <jni.h>
#include <FilterGroup.h>
#include <Filter.h>

using namespace GCVBase;

extern "C" {
jlong Java_com_example_filterlibrary_FilterGroup_nativeFilterGroupInit(JNIEnv * env, jobject obj){
    FilterGroup * filterGroup = new FilterGroup();
    return (jlong)filterGroup;
}

void Java_com_example_filterlibrary_FilterGroup_nativeFilterGroupAddFilter(JNIEnv * env, jobject obj, jlong filterGroupAddress, jlong filterAddress){
    Filter * filter = (Filter *)filterAddress;
    FilterGroup * filterGroup = (FilterGroup *)filterGroupAddress;

    filterGroup->addFilter(filter);
}

void Java_com_example_filterlibrary_FilterGroup_nativeFilterGroupRemoveFilter(JNIEnv * env, jobject obj, jlong filterGroupAddress, jlong filterAddress){
    Filter * filter = (Filter *)filterAddress;
    FilterGroup * filterGroup = (FilterGroup *)filterGroupAddress;

    filterGroup->removeFilter(filter);
}

}