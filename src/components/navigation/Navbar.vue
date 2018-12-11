<template>
  
  <div>   
    <v-navigation-drawer
      v-model="drawer"
      app
      disable-resize-watcher
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
              <v-icon>{{ item.icon }}</v-icon>
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
        v-if="$vuetify.breakpoint.xsOnly"
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
      </v-toolbar-items>

      <template
        v-if="$vuetify.breakpoint.xsOnly && relatedItem($route.name)"
      >
        <v-spacer />
        <v-toolbar-items>
          <v-btn
            flat
            :to="relatedItem($route.name).componentName"
            @click="itemClicked(relatedItem($route.name))"
          >
            {{ relatedItem($route.name).title }}
          </v-btn>
        </v-toolbar-items>

      </template>
      
    </v-toolbar>
      
  </div>
    
</template>

<script>
export default {
  props: {
  },
  data() {
    return {
      drawer: false,
      appTitle: "Image Aligner"
    };
  },
  computed: {
    itemsLeft() {
      return [
        {
          title: "Info",
          componentName: "info",
          func: null,
          icon: "device_hub"
        },
        {
          title: "Matcher",
          componentName: "matcher",
          func: null,
          icon: "vpn_key"
        },
        {
          title: "Comparer",
          componentName: "comparer",
          func: null,
          icon: "compare"
        },
        {
          title: "Settings",
          componentName: "settings",
          func: null,
          icon: "settings"
        }
      ];
    },
    itemsRight() {
      return [];
    },
    nonNavItems() {
      return [];
    },
    navItems() {
      return [...this.itemsLeft, ...this.itemsRight];
    },
    allItems() {
      return [...this.itemsLeft, ...this.itemsRight, ...this.nonNavItems];
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
