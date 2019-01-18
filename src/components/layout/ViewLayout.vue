<template>
  <v-layout
    align-center
    justify-center
  >
    <v-flex
      xs11
      sm11
      md11
      lg10
      xl10
    >
      <app-view-title
        v-if="title"
        :title="title"
      > 
        <template
          slot="leftElem"
        >
          <slot name="leftElem" />
        </template>
        
        <template
          slot="rightElem"
        >
          <slot name="rightElem" />
        </template>
      </app-view-title>
      
      <slot />
    </v-flex>
    
  </v-layout>
</template>

<script>

import ViewTitle from '@/components/layout/ViewTitle';

export default {
  components: {
    'appViewTitle': ViewTitle
  },
  props: {
    title: {
      type: String,
      default: null
    },
  }
}
</script>

<style>

</style>

