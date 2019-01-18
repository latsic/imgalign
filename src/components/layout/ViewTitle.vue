<template>
  <div>
    <v-layout
      justify-space-between
      align-center
    >
      <v-layout
        align-center
        justify-start
      >
        <h3 class="text-xs-left">
          {{ title }}
        </h3>
     
        <div>
          <slot name="leftElem" />
        </div>
      </v-layout>
          
      <div
        :style="{position:'relative'}"
      >
        <div
          :style="{
            position:'absolute',
            top: '0',
            left: '0',
            transform: 'translate(-100%, -50%)',
          }"
        >
          <slot name="rightElem" />
        </div>
          
      </div>
    </v-layout>

    <app-view-spacer
      :mtop="'0'"
    />
  </div>
</template>

<script>

import ViewSpacer from '@/components/layout/ViewSpacer';

export default {
  components: {
    'appViewSpacer': ViewSpacer
  },
  props: {
    title: {
      type: String,
      required: true
    }
  }
  
}
</script>

<style scoped>

</style>
