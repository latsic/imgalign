import Vue from 'vue';
import Router from 'vue-router';
import Settings from '@/views/Settings';
import Info from '@/views/Info';
import Matcher from '@/views/Matcher';
import Comparer from '@/views/Comparer';

Vue.use(Router)

export default new Router({
  mode: 'history',
  base: process.env.BASE_URL,
  routes: [
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
    }
  ]
})
