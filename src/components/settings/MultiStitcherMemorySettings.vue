<template>
  <v-layout
    row
    justify-start
    wrap
    align-center
  >
    <v-flex
      v-for="param of params"
      :key="param.id"
      xs6
      sm6
      pa-2
    >
      <v-switch
        v-if="param.type == valueTypes.bool"
        :value="param.value > 0 ? true : false"
        :input-value="param.value > 0 ? true : false"
        :label="paramName(param.id)"
        :style="{margin: '0', padding: '0'}"
        hide-details
        @change="value => changed(param.id, value ? 1 : 0 )"
        class="switch-text-small"
      />
      <v-slider
        v-if="param.type == valueTypes.rangeSquareRoot"  
        always-dirty
        v-bind="additionalSliderAttributes(param, 'px')"
        :value="sliderValueSqrt(param.value)"
        :max="sliderValueSqrt(param.range.max)"
        :min="sliderValueSqrt(param.range.min)"
        :step="20"
        :style="{'margin-top': '0', 'margin-bottom': '0'}"
        @change="value => $emit('change', { id: param.id, value: value * value })"
      />
      <v-slider
        v-if="param.type == valueTypes.range"
        always-dirty
        v-bind="additionalSliderAttributes(param, '')"
        :value="param.value"
        :max="param.range.max"
        :min="param.range.min"
        :step="1"
        :style="{'margin-top': '0', 'margin-bottom': '0'}"
        @change="value => $emit('change', { id: param.id, value })"
      />
    </v-flex>
  </v-layout>
</template>

<script>

import { valueTypes, ParamUtils } from '@/models/constants/params';

export default {
  props: {
    params: {
      type: Array,
      required: true
    }
  },
  data() {
    return {
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
    changed(id, value) {
      const valueNumber = Number(value);
      if(!isNaN(valueNumber)) {
        this.$emit('change', { id, value: valueNumber });
      }
    },
    additionalSliderAttributes(param, suffix) {
      // if(this.$vuetify.breakpoint.name == "xs") {
        return {
          hint: `${this.paramName(param.id)}: ${param.value}${suffix ? ' ' + suffix : ''}`,
          'persistent-hint': true,
          label: '',
          'hide-details': false
        }
      // }
      // else {
        // return {
        //   label: `${this.paramName(param.id)} ${param.value}${suffix ? ' ' + suffix : ''}`,
        //   'persistent-hint': false,
        //   hint: null,
        //   'hide-details': true
        // }
      // }
    },
    sliderValueSqrt(value) {
      return Math.round(Math.sqrt(value));
    }
  }
}
</script>

<style scoped>
  .switch-text-small >>> label {
    font-size: 0.7em;
    font-weight: 600; 
  }
</style>
