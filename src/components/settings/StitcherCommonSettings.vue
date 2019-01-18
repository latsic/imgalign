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
      />
      <!-- <v-slider
        v-if="param.type == valueTypes.range"  
        always-dirty
        :value="param.value"
        :hint="`${paramName(param.id)} ${param.value} px`"
        persistent-hint
        :max="param.range.max"
        :min="param.range.min"
        :step="1"
        :style="{
          'margin-top': '0',
          'margin-bottom': '0'
        }"
        @change="value => $emit('change', { id: param.id, value })"
      />
      <app-select
        v-if="param.type == valueTypes.discrete"
        :param="param"
        @change="changedDiscrete"
      /> -->
    </v-flex>
  </v-layout>
</template>

<script>

import { valueTypes, ParamUtils } from '@/models/constants/params';
// import Select from '@/components/settings/Select';

export default {
  // components: {
  //   'AppSelect': Select
  // },
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
    // paramText(id) {
    //   return ParamUtils.getParamText(id);
    // },
    // changedDiscrete(obj) {
    //   this.$emit('change', obj);
    // },
    changed(id, value) {
      const valueNumber = Number(value);
      if(!isNaN(valueNumber)) {
        this.$emit('change', { id, value: valueNumber });
      }
    }
  }
}
</script>

<style scoped>
/* .slider {
    margin-top: 0;
    margin-bottom: 0;
    border: 2px solid;
    border-radius: 0.3rem 0.3rem;
    padding: 0.7rem 0.7rem
  } */
</style>
