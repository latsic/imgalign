<template>
  <app-view-layout
    :title="'Settings'"
  >
    <template
      slot="rightElem"
    > 
      <div
        class="caption set-default-action"
        :class=" {
          'set-default-action-enabled': !$store.getters['settings/equalsDefault']
        }"
        :style="{
          'white-space': 'nowrap',
          color: $store.getters['settings/equalsDefault'] ? $vuetify.theme.secondary : $vuetify.theme.primary,
          'padding-right': '0.2rem'
        }"
        @click="$store.getters['settings/equalsDefault'] ? () => {} : $store.dispatch('settings/default')"
      >
        <strong>▶ Set Default</strong>
      </div>

    </template>

    <app-basic-settings
      :params="params(groupKeyBasicSettings)"
      @change="obj => $store.dispatch('settings/param', obj)"
    />
    <app-view-spacer
      :show-line="true"
      :mtop="'1.5rem'"
    />

    <app-view-spacer
      :show-line="false"
    />
    
    <app-view-title
      title="Advanced detector/descriptor"
    />

    <v-tabs
      :value="$store.getters['settings/activeTabIndex'] || 0"
      
      show-arrows
      light
      color="warning"
      @change="index => $store.dispatch('settings/activeTabIndex', index)"
    >
      <v-tab
        v-for="(groupKey, index) of groupKeysDetectorSettings"
        :key="index"
        ripple
      >
        {{ groupKey }}
      </v-tab>
      <v-tab-item
        v-for="(groupKey, index) of groupKeysDetectorSettings"
        :key=index
        lazy
      >
        <app-detector-settings
          :params="params(groupKey)"
          @changed="obj => $store.dispatch('settings/param', obj)"
        />
      </v-tab-item>
    </v-tabs>

    <app-view-spacer
      :show-line="false"
    />

    <app-view-title
      title="Advanced match filter"
    />

    <app-match-filter
      :params="params(groupKeyMatchFilter)"
      @changed="obj => $store.dispatch('settings/param', obj)"
    />

    <app-view-spacer
      :show-line="false"
    />
    
    <app-view-title
      title="Various other settings"
    />

    <app-other-settings
      :params="params(groupKeyOtherSettings)"
      @changed="obj => $store.dispatch('settings/param', obj)"
    />

    <app-view-spacer
      :show-line="false"
    />

    <app-view-title
      title="Logging settings"
    />

    <app-log-settings
      :params="params(groupKeyLogSettings)"
      @changed="obj => $store.dispatch('settings/param', obj)"
    />


  </app-view-layout>
</template>

<script>

import ViewLayout from '@/components/layout/ViewLayout';
import ViewTitle from '@/components/layout/ViewTitle';
import ViewSpacer from '@/components/layout/ViewSpacer';
import BasicSettings from '@/components/settings/BasicSettings';
import DetectorSettings from '@/components/settings/DetectorSettings';
import MatchFilter from '@/components/settings/MatchFilter';
import OtherSettings from '@/components/settings/OtherSettings';
import LogSettings from '@/components/settings/LogSettings';

export default {
  components: {
    'AppViewLayout': ViewLayout,
    'AppViewSpacer': ViewSpacer,
    'AppViewTitle': ViewTitle,
    'AppBasicSettings': BasicSettings,
    'AppDetectorSettings': DetectorSettings,
    'AppMatchFilter': MatchFilter,
    'AppOtherSettings': OtherSettings,
    'AppLogSettings': LogSettings
  },
  data() {
    return {
      groupKeysDetectorSettings: ['sift', 'surf', 'orb', 'brisk', 'kaze', 'akaze'],
      groupKeyOtherSettings: 'other',
      groupKeyBasicSettings: 'basic',
      groupKeyMatchFilter: 'matchFilter',
      groupKeyLogSettings: 'logging',
      activeTabIndex: 0
    }
  },
  created() {
    this.setInitialTabIndex();
  },
  computed: {
    settings() {
      return this.$store.getters['settings/settings'];
    }
  },
  methods: {
    params(groupKey) {
      return this.settings.paramsByGroupKey(groupKey);
    },
    tabChanged(index) {
      this.activeTabIndex = index;
    },
    setInitialTabIndex() {
      
      if(this.$store.getters['settings/activeTabIndex'] !== null) return; 
      
      const activeDetGroupKey = this.$store.getters['settings/getCurrentDetTypeGroupKey'];
      const groupIndex = this.groupKeysDetectorSettings.findIndex(groupKey => groupKey == activeDetGroupKey);
      
      if(groupIndex >= 0) {
        this.$store.dispatch('settings/activeTabIndex', groupIndex)
      }
    }
  }
}
</script>

<style scoped>

  .set-default-action {
    opacity: 0.2
  }

  .set-default-action-enabled {
    cursor: pointer;
    opacity: 1;
  }
  .set-default-action-enabled:hover {
    color: red !important;
  }
</style>
