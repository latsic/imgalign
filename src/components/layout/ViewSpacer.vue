<template>
  <div
    :style="{
      'border-top': `1px solid ${lineColor}`,
      'margin-top': mtop,
      'margin-bottom': mbottom}"
  />
</template>

<script>
export default {
  props: {
    mtop: {
      type: String,
      default: '1rem'
    },
    mbottom: {
      type: String,
      default: '1rem'
    },
    showLine: {
      type: Boolean,
      default: true
    }
  },
  computed: {
   lineColor() {
     return this.showLine
      ? this.$vuetify.theme.secondary
      : '#00000000'
   } 
  }
}
</script>

<style scoped>

</style>
