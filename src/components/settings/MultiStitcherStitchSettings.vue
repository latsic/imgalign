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
      <v-slider
        v-if="param.type == valueTypes.range"  
        always-dirty
        :value="param.value"
        :hint="`${paramName(param.id)} ${param.value}`"
        persistent-hint
        :max="param.range.max"
        :min="param.range.min"
        :step="0.01"
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
    }
  }
}
</script>

<style scoped>
</style>
