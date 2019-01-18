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
      <app-select
        v-if="param.type == valueTypes.discrete"
        :param="param"
        @change="changedDiscrete"
      />
    </v-flex>
  </v-layout>
</template>

<script>

import { valueTypes } from '@/models/constants/params';
import Select from '@/components/settings/Select';

export default {
  components: {
    'AppSelect': Select
  },
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
    changedDiscrete(obj) {
      this.$emit('change', obj);
    }
  }
}
</script>

<style scoped>
</style>
