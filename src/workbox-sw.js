
workbox.core.setCacheNameDetails({prefix: "imgalign"});


self.__precacheManifest = [].concat(self.__precacheManifest || []);
workbox.precaching.suppressWarnings();

workbox.routing.registerNavigationRoute('/imgalign/index.html');
workbox.precaching.precacheAndRoute(self.__precacheManifest, {});

workbox.googleAnalytics.initialize();

self.addEventListener('install', () => {
  self.skipWaiting();
});