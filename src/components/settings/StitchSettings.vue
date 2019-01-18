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
      <v-text-field
        v-if="param.type == valueTypes.number"
        :name="param.id"
        :label="paramName(param.id)"
        :value="param.value"
        :rules="[rules.required]"
        :type="'number'"
        :hint="paramText(param.id)"
        @change="value => changed(param.id, value)"
        :disabled="isDisabled(param)"
      />
      <v-switch
        v-if="param.type == valueTypes.bool"
        :value="param.value > 0 ? true : false"
        :input-value="param.value > 0 ? true : false"
        :label="paramName(param.id)"
        :hint="paramText(param.id)"
        @change="value => changed(param.id, value ? 1 : 0 )"
      />
      <v-slider
        v-if="param.type == valueTypes.range"  
        always-dirty
        :value="param.value"
        :hint="`${paramName(param.id)} ${param.value} px`"
        persistent-hint
        :max="param.range.max"
        :min="param.range.min"
        :step="1"
        :style="{'margin-top': '0', 'margin-bottom': '0'}"
        @change="value => $emit('changed', { id: param.id, value })"
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
      rules: {
        required: value => value.toString().length > 0 && !isNaN(Number(value)) || 'Required'
      }
    }
  },
  computed: {
    valueTypes() {
      return valueTypes;
    }
  },
  methods: {
    isDisabled(param) {

      const enabledIfId = ParamUtils.getParamEnabledIfId(param.id);
      if(!enabledIfId) return true;

      for(const p of this.params) {
        if(p.id == enabledIfId) {
          return p.value == 0;
        }
      }
      return false;
    },
    paramName(id) {
      return ParamUtils.getParamName(id);
    },
    paramText(id) {
      return ParamUtils.getParamText(id);
    },
    changed(id, value) {
      const valueNumber = Number(value);
      if(!isNaN(valueNumber)) {
        this.$emit('changed', { id, value: valueNumber });
      }
    }
  }
}
</script>

<style scoped>

</style>
