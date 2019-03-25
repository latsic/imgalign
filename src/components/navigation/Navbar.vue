<template>
  
  <div>   
    <v-navigation-drawer
      v-model="drawer"
      app
      disable-resize-watcher
      temporary

    >
      <v-list dense>
        <template
          v-for="item in navItems"
        >
          <v-list-tile
            :key="item.title"
            :to="item.componentName ? {name: item.componentName} : null"
            :color="item.errorColor ? item.errorColor : undefined"
            @click="itemClicked(item)"
          >
            <v-list-tile-action>

              <v-icon
                v-if="!item.isSvg"  
              >
                {{ item.icon }}
              </v-icon>
              <v-layout
                v-else
                >
                <img :src="item.icon" alt="logo" width='20px'/>
              </v-layout>
            </v-list-tile-action>
            <v-list-tile-content>
              <v-list-tile-title v-text="item.title" />
            </v-list-tile-content>
          </v-list-tile>
          <v-divider
            :key="item.title + '_2'"
          />
        </template>
      </v-list>
    </v-navigation-drawer>
    <v-toolbar
      app
      flat
      dark
      dense
    >
      <v-toolbar-side-icon
        v-if="$vuetify.breakpoint.xsOnly || $vuetify.breakpoint.smAndUp"
        @click.stop="drawer = !drawer"
      />
    
      <v-toolbar-items
        v-if="$vuetify.breakpoint.smAndUp"
      >
        <v-btn
          v-for="item of itemsLeft"
          :key="item.title"
          flat
          :to="item.componentName ? {name: item.componentName} : null"
          @click="itemClicked(item)"
        >
          {{ item.title }}
        </v-btn>
      </v-toolbar-items>

      <template
        v-if="$vuetify.breakpoint.smAndUp"
      >
        <v-spacer />
        <v-toolbar-title
          v-if="$vuetify.breakpoint.mdAndUp"
          :class="{'secondary--text': !nonNavRouteTitle}"
        >
          {{ nonNavRouteTitle ? nonNavRouteTitle : appTitle }}
        </v-toolbar-title>
        <v-toolbar-title
          v-else
          :class="{'secondary--text': !nonNavRouteTitle}"
        >
          {{ nonNavRouteTitle ? nonNavRouteTitle : '' }}
        </v-toolbar-title>
        <v-spacer />
      </template>

      <template
        v-if="$vuetify.breakpoint.xsOnly"
      >
        <v-toolbar-title class="title">
          {{ routeTitle }}
        </v-toolbar-title>
      </template>

      <v-toolbar-items
        v-if="$vuetify.breakpoint.smAndUp"
      >
        <v-btn
          v-for="item of itemsRight"
          :key="item.title"
          flat
          :to="item.componentName ? {name: item.componentName} : null"
          :color="item.errorColor ? item.errorColor : undefined"
          @click="itemClicked(item)"
        >
          {{ item.title }}
        </v-btn>
        <v-layout
          v-if="busy"
          align-center
        >
          <app-spinner
            :color="busyColor"
          />
        </v-layout>
        <v-layout
          justify-end
          align-center
          v-else
          >
          <img :src="logo" alt="logo" width='30px'/>
        </v-layout>
      </v-toolbar-items>

      <template
        v-if="$vuetify.breakpoint.xsOnly"
      >
        <v-spacer />
        <v-toolbar-items>
          <v-layout
            align-center
            v-if="busy"
          >
            <app-spinner
              :color="busyColor"
            />
          </v-layout>
          <v-layout
            align-center
            v-else
          >
            <img :src="logo" alt="logo" width='30px'/>
          </v-layout>
        </v-toolbar-items>
      </template>
      
    </v-toolbar>
      
  </div>
    
</template>

<script>

import Spinner from '@/components/gui/Spinner';
import LogoSvg from '@/assets/logo_200x200.png';

export default {
  components: {
    'AppSpinner': Spinner
  },
  props: {
    busyWorker: {
      type: Boolean,
      default: false
    },
    workerReady: {
      type: Boolean,
      default: true
    },
    busyInput: {
      type: Boolean,
      default: false
    }
  },
  data() {
    return {
      drawer: false,
      appTitle: 'Image Aligner'
    };
  },
  computed: {
    itemsLeft() {
      return [
        
        {
          title: 'Matcher',
          componentName: 'matcher',
          func: null,
          icon: 'filter_none'
        },
        {
          title: 'Comparer',
          componentName: 'comparer',
          func: null,
          icon: 'fingerprint',
          isSvg: false
        },
        {
          title: 'Stitcher',
          componentName: 'stitcher',
          func: null,
          icon: 'broken_image'
        },
        {
          title: 'MultiStitcher',
          componentName: 'multistitcher',
          func: null,
          icon: 'burst_mode'
        },
        
        
      ];
    },
    itemsRight() {
      return [];
    },
    nonNavItems() {
      return [];
    },
    navItems() {
      return [...this.itemsLeft, ...this.itemsRight, ...this.itemsXsOnly];
    },
    itemsXsOnly() {
      return [
        {
          title: "Info",
          componentName: "info",
          func: null,
          icon: "info"
        },
        {
          title: "Global settings",
          componentName: "settings",
          func: null,
          icon: "settings"
        },
        {
          title: "Log",
          componentName: "logoutput",
          func: null,
          icon: "reorder"
        }
      ];
    },
    allItems() {
      return [...this.itemsXsOnly, ...this.itemsLeft, ...this.itemsRight, ...this.nonNavItems];
    },
    routeTitle() {
      for (const item of this.allItems) {
        if (item.componentName == this.$route.name) {
          return item.title;
        }
      }
      return "";
    },
    nonNavRouteTitle() {
      for(const item of this.nonNavItems) {
        if(item.componentName == this.$route.name && item.title) {
          return item.title;
        }
      }
      return null;
    },
    relatedItems() {
      return {
        ['matcher']: this.allItems.find(item => item.componentName == 'settings'),
        ['settings']: this.allItems.find(item => item.componentName == 'matcher')
      }
    },
    busy() {
      return this.busyWorker || this.busyInput;
    },
    busyColor() {
      if(this.busyWorker || !this.workerReady) {
        return this.$vuetify.theme.accent;
      }
      return this.$vuetify.theme.error;
    },
    logo() {
      return LogoSvg;
    }
  },
  methods: {
    itemClicked(item) {
      if(item.func) {
        item.func();
      }
    },
    relatedItem(name) {
      return this.relatedItems[name];
    }
  }
};
</script>

<style scoped>
</style>
