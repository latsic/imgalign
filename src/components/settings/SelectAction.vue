<template>
  <v-layout>
    <app-select
      :param="param"
      @change="value => $emit('select-change', value)"
    />

    <v-btn
      icon
      :disabled="actionDisabled"
      :color="'primary lighten-1'"
      small
      @click="$emit('action-click')"
    >
      <v-icon>cached</v-icon>
    </v-btn>
  </v-layout>
</template>

<script>

import Select from '@/components/settings/Select';

export default {
  components: {
    'AppSelect': Select
  },
  props: {
    param: {
      type: Object,
      required: true
    },
    actionDisabled: {
      type: Boolean,
      default: false
    }
  }
}
</script>

<style scoped>

</style>


