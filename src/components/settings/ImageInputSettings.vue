<template>
  <v-layout
    row
    justify-start
    wrap
    align-baseline
  >
    <v-flex
      v-for="param of params"
      :key="param.id"
      xs12
      sm6
      pa-2
    >
      <v-slider
        v-if="param.type == valueTypes.rangeSquareRoot"  
        always-dirty
        :value="sliderValueSqrt(param.value)"
        :hint="`${paramName(param.id)}: ${param.value} px`"
        persistent-hint
        :max="sliderValueSqrt(param.range.max)"
        :min="sliderValueSqrt(param.range.min)"
        :step="20"
        :style="{'margin-top': '0', 'margin-bottom': '0'}"
        @change="value => $emit('changed', { id: param.id, value: value * value })"
      />
    </v-flex>
  </v-layout>
</template>

<script>

import { ParamUtils, valueTypes } from '@/models/constants/params';

export default {
  
  props: {
    params: {
      type: Array,
      required: true
    }
  },
  computed: {
    valueTypes() {
      return valueTypes;
    }
  },
  methods: {
    paramName(id) {
      return ParamUtils.getParamName(id);
    },
    sliderValueSqrt(value) {
      return Math.round(Math.sqrt(value));
    }
  }
}
</script>

<style scoped>

</style>
