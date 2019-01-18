<template>
  <v-layout
    row
    justify-start
    wrap
    align-baseline
  >
    <v-flex
      v-for="paramBridge of getParamsMapped(false)"
      :key="paramBridge.param.id"
      xs4
      sm4
      pa-2
    >
      <v-text-field
        :name="paramBridge.param.id"
        :label="paramName(paramBridge.param.id)"
        :placeholder="paramBridge.placeHolder()"
        :value="paramBridge.getValue()"
        :type="'number'"
        :hint="paramText(paramBridge.param.id)"
        @change="value => changed(paramBridge, value)"
      />
    </v-flex>
  </v-layout>
</template>

<script>

import { valueTypes, ParamUtils, paramTypes } from '@/models/constants/params';
import { ParamBridgeNumberAuto } from '@/utilities/ParamBridgeNumberAuto';


export default {
  components: {
    
  },
  props: {
    params: {
      type: Array,
      required: true
    },
    fieldOfViewInitial1: {
      type: Number,
      default: null
    },
    fieldOfViewInitial2: {
      type: Number,
      default: null
    },
    getParamById: {
      type: Function,
      required: true
    }

  },
  data() {
    return {
      rules: {
        numberOrEmpty: value => value.toString().length == 0 || !isNaN(Number(value)) || 'Number or empty',
        required: value => value.toString().length > 0 && !isNaN(Number(value)) || 'Required'
      },
      paramsMapped: []
    }
  },
  watch: {
    fieldOfViewInitial1() {
      this.calcParamsMapped();
    },
    fieldOfViewInitial2() {
      this.calcParamsMapped();
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
    paramText(id) {
      return ParamUtils.getParamText(id);
    },
    changed(paramBridge, value) { 
      paramBridge.setValue(value, obj => this.$emit('change', obj));
    },
    calcParamsMapped() {
      this.paramsMapped = [];

      for(const param of this.params) {
        
        if(param.type != valueTypes.numberAuto) {
          continue;
        }

        let externDefault = null;
        if(param.id == paramTypes.stitch_fieldOfView1.id) {
          externDefault = this.fieldOfViewInitial1;
        }
        else if(param.id == paramTypes.stitch_fieldOfView2.id) {
          externDefault = this.fieldOfViewInitial2;
        }
        
        const flagParam = param.flagId
          ? this.getParamById(param.flagId)
          : null;

        this.paramsMapped.push(new ParamBridgeNumberAuto(
          param,
          flagParam,
          externDefault ? () => externDefault : null));
      }
      return this.paramsMapped;
    },
    getParamsMapped(doRecalc) {
      if(doRecalc || this.paramsMapped.length == 0) {
        this.calcParamsMapped();
      }
      return this.paramsMapped;
    }
  }
}
</script>

<style scoped>

</style>
