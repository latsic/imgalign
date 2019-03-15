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
        :disabled="isDisabled(param)"
        hide-details
        @change="value => changed(param.id, value ? 1 : 0 )"
        class="switch-text-small"
      />
      <v-slider
        v-if="param.type == valueTypes.range"  
        always-dirty
        v-bind="additionalSliderAttributes(param)"
        :value="sliderValue(param)"
        :max="param.range.max"
        :min="param.range.min"
        :step="param.step ? param.step : '0.01'"
        :style="{'margin-top': '0', 'margin-bottom': '0'}"
        :disabled="isDisabled(param)"
        @change="value => sliderChanged(param, value)"
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
    isDisabled(param) {

      const enabledIfId = ParamUtils.getParamEnabledIfId(param.id);
      if(!enabledIfId) return false;

      for(const p of this.params) {
        if(p.id == enabledIfId) {
          return p.value == 0;
        }
      }
      return false;
    },
    additionalSliderAttributes(param) {

      const paramValue = param.value === param.range.maxReplace ? `> ${param.range.max}` : param.value

      if(this.$vuetify.breakpoint.name == "xs") {
        return {
          hint: `${this.paramName(param.id)}: ${paramValue}`,
          'persistent-hint': true,
          label: '',
          'hide-details': false
        }
      }
      else {
        return {
          label: `${this.paramName(param.id)}: ${paramValue}`,
          'persistent-hint': false,
          hint: null,
          'hide-details': true
        }
      }
    },
    sliderValue(param) {
      if(param.range.maxReplace != undefined && param.value == param.range.maxReplace) {
        return param.range.max;
      }
      return param.value;
    },
    sliderChanged(param, value) {
      let emitValue = value;
      if(param.range.maxReplace != undefined && value == param.range.max) {
        emitValue = param.range.maxReplace;
      }
      this.$emit('change', { id: param.id, value: emitValue });
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
