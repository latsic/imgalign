import Vue from 'vue';
import Router from 'vue-router';
import Settings from '@/views/Settings';
import Info from '@/views/Info';
import Matcher from '@/views/Matcher';
import Comparer from '@/views/Comparer';
import Stitcher from '@/views/Stitcher';
import MultiStitcher from '@/views/MultiStitcher';
import LogOutput from '@/views/LogOutput';

Vue.use(Router)

export default new Router({
  mode: 'history',
  base: process.env.BASE_URL,
  routes: [
    {
      path: '/index.html', redirect: { name: 'matcher' }
    },
    {
      path: '/', redirect: { name: 'info'}
    },
    {
      path: '/settings',
      name: 'settings',
      component: Settings
    },
    {
      path: '/info',
      name: 'info',
      component: Info
    },
    {
      path: '/matcher',
      name: 'matcher',
      component: Matcher
    },
    {
      path: '/comparer',
      name: 'comparer',
      component: Comparer
    },
    {
      path: '/stitcher',
      name: 'stitcher',
      component: Stitcher
    },
    {
      path: '/multistitcher',
      name: 'multistitcher',
      component: MultiStitcher
    },
    {
      path: '/logoutput',
      name: 'logoutput',
      component: LogOutput
    }
  ]
})
