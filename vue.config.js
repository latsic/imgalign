
// const CopyWebpackPlugin = require('copy-webpack-plugin')

module.exports = {
  publicPath: process.env.NODE_ENV === 'production'
    ? '/imgalign/'
    : '/',
  pwa: {
    name: 'Image Needle',
    themeColor: '#000000',
    msTileColor: '#000000',
    appleMobileWebAppCapable: 'yes',
    appleMobileWebAppStatusBarStyle: 'black',
  
    // configure the workbox plugin
    // workboxPluginMode: 'GenerateSW',
    // workboxOptions: {
    // }
    workboxPluginMode: 'InjectManifest',
    workboxOptions: {
      swSrc: 'src/workbox-sw.js',
      swDest: 'workbox-sw.js',
      // modifyUrlPrefix: {
      //   // Remove a '/dist' prefix from the URLs:
      //   '/': process.env.NODE_ENV === 'production' ? '/imgalign/' : '/'
      // }
    }
  }
}