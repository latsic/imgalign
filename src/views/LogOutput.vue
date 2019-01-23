<template>
  <app-view-layout
    :title="'Latest log messages'"
  >
    <div
      v-for="(msg, index) of $store.getters['logs/errorMessages'].slice().reverse()"
      :v-text="msg"
      :key="index"
      :style="{
        margin: '0',
        padding: '0',
        'font-size': '0.7em',
        'text-align': 'left',
        color: $vuetify.theme.error
      }"
      >
      {{ msg }}
    </div>
    <app-view-spacer
      v-if="$store.getters['logs/errorMessages'].length > 0"
    />
    <div
      v-for="(msg, index) of $store.getters['logs/messages'].slice().reverse()"
      :v-text="msg"
      :key="index + 'error'"
      :style="{
        margin: '0',
        padding: '0',
        'font-size': '0.7em',
        'text-align': 'left',
        color: index % 2 == 0 ? $vuetify.theme.secondary : $vuetify.theme.primary }"
      >
      {{ msg }}
    </div>
  </app-view-layout>
</template>

<script>

import ViewLayout from '@/components/layout/ViewLayout';
import ViewSpacer from '@/components/layout/ViewSpacer';

export default {
  components: {
    'AppViewSpacer': ViewSpacer,
    'AppViewLayout': ViewLayout
  }
}
</script>

<style scoped>

</style>
