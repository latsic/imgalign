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
      @change="paramChanged"
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
      :value="$store.getters['settings/activeTabIndexDetector'] || 0"
      
      show-arrows
      light
      color="warning"
      @change="index => $store.dispatch('settings/activeTabIndexDetector', index)"
    >
      <v-tab
        v-for="(groupKey, index) of groupKeysDetectorSettings"
        :key="index"
        ripple
      >
        {{ paramGroupName(groupKey) }}
      </v-tab>
      <v-tab-item
        v-for="(groupKey, index) of groupKeysDetectorSettings"
        :key=index
        lazy
      >
        <app-detector-settings
          :params="params(groupKey)"
          @changed="paramChanged"
        />
      </v-tab-item>
    </v-tabs>

    <app-view-spacer
      :show-line="false"
    />
    <app-view-title
      title="Image and other settings"
    />
    <app-other-settings
      :params="params(groupKeyOtherSettings)"
      @changed="paramChanged"
    />

    <app-view-spacer
      :show-line="false"
    />
    <app-view-title
      title="Advanced match filter"
    />
    <app-match-filter
      :params="params(groupKeyMatchFilter)"
      @changed="paramChanged"
    />

    <app-view-spacer
      :show-line="false"
    />
    <app-view-title
      title="Logging settings"
    />
    <app-log-settings
      :params="params(groupKeyLogSettings)"
      @changed="logParamChanged"
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
//import StitchSettings from '@/components/settings/StitchSettings';
import { paramTypes, paramGroups } from '@/models/constants/params';

export default {
  components: {
    'AppViewLayout': ViewLayout,
    'AppViewSpacer': ViewSpacer,
    'AppViewTitle': ViewTitle,
    'AppBasicSettings': BasicSettings,
    'AppDetectorSettings': DetectorSettings,
    'AppMatchFilter': MatchFilter,
    'AppOtherSettings': OtherSettings,
    'AppLogSettings': LogSettings,
    //'AppStitchSettings': StitchSettings
  },
  data() {
    return {
      groupKeysDetectorSettings: ['sift', 'surf', 'orb', 'brisk', 'kaze', 'akaze'],
      groupKeyOtherSettings: 'other',
      groupKeyBasicSettings: 'basic',
      groupKeyMatchFilter: 'matchFilter',
      groupKeyLogSettings: 'logging',
      groupKeyStitchSettings: 'stitching',
      activeTabIndexDetector: 0
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
      this.activeTabIndexDetector = index;
    },
    setInitialTabIndex() {
      
      if(this.$store.getters['settings/activeTabIndexDetector'] !== null) return; 
      
      const activeDetGroupKey = this.$store.getters['settings/getCurrentDetTypeGroupKey'];
      const groupIndex = this.groupKeysDetectorSettings.findIndex(groupKey => groupKey == activeDetGroupKey);
      
      if(groupIndex >= 0) {
        this.$store.dispatch('settings/activeTabIndexDetector', groupIndex)
      }
    },
    paramChanged(obj) {
      this.$store.dispatch('settings/param', obj);
      if(obj.id == paramTypes.alignSelectionOverlay.id) {

        this.$store.dispatch('worker/requestImage');
      }
      this.$store.dispatch('worker/multiStitchResetWorkerData');
    },
    logParamChanged(obj) {
      this.$store.dispatch('settings/param', obj);
    },
    paramGroupName(key) {
      return paramGroups[key].name;
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
