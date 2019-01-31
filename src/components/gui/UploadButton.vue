<template>
  <div
    class="upload-btn"
  > 
    <input
      :id="id"
      type="file"
      :name="name"
      :value="value"
      :accept="accept"
      :multiple="isMultiple"
      @change="fileChanged"
      @click="fileSelectionStarted"
    >
    <label 
      :for="id"
      :class="`v-btn ${classes}${color} upload-btn`"
      :style="{padding: '0', margin: '0', 'text-transform': 'none !important'}"
    >
      <slot name="icon-left" />
      {{ icon ? '' : title }}
      <slot name="icon" />
    </label>
  </div>
</template>



<script>

  import { UserAgentInfo } from '@/utilities/UserAgentInfo';

  export default {
    props: {
      accept: {
        default: '*',
        type: String
      },
      block: {
        default: false,
        type: Boolean
      },
      depressed: {
        default: false,
        type: Boolean
      },
      fileChangedCallback: {
        default: undefined,
        type: Function
      },
      color: {
        default: 'primary',
        type: String
      },
      disabled: {
        default: false,
        type: Boolean
      },
      flat: {
        default: false,
        type: Boolean
      },
      hover: {
        default: true,
        type: Boolean
      },
      icon: {
        default: false,
        type: Boolean
      },
      large: {
        default: false,
        type: Boolean
      },
      loading: {
        default: false,
        type: Boolean
      },
      multiple: {
        default: false,
        type: Boolean
      },
      name: {
        default: 'uploadFile',
        type: String
      },
      outline: {
        default: false,
        type: Boolean
      },
      ripple: {
        default: true,
        type: Boolean
      },
      round: {
        default: false,
        type: Boolean
      },
      small: {
        default: false,
        type: Boolean
      },
      title: {
        default: 'Upload',
        type: String
      },
      uniqueId: {
        default: false,
        type: Boolean
      }
    },
    computed: {
      id () {
        return this.uniqueId ? `${this._uid}uploadFile` : 'uploadFile'
      },
      classes () {
        const classes = {
          'v-btn--block': this.block,
          'v-btn--flat': this.flat,
          'upload-btn-hover': this.hover,
          'v-btn--icon': this.icon,
          'v-btn--large': this.large,
          'v-btn--loading': this.loading,
          'v-btn--outline': this.outline,
          'v-btn--round': this.round,
          'v-btn--small': this.small,
          'v-btn--disabled': this.disabled,
          'v-btn--depressed': this.depressed
        }
        let classString = "";
        for(let key in classes) {
          if(classes[key]) {
            classString += `${key} `
          }
        }
        return classString;
      },
      isMultiple() {
        return this.multiple && !UserAgentInfo.instance.isGeckoOnAndroid();
      }
    },
    data() {
      return {
        value: '',
        fileChangedCalled: false
      }
    },
    methods: {
      fileChanged(e) {
        this.fileChangedCalled = true;
        this.fileSelectionStarted();
        if(e) {
          if(this.fileChangedCallback) {
            if(e.target.files) {
              if(this.multiple && !this.isMultiple && e.target.files[0]) {
                this.fileChangedCallback([e.target.files[0]]);
              }
              else if(this.isMultiple) {
                this.fileChangedCallback(e.target.files);
              }
              else if(!this.multiple && e.target.files[0]) {
                this.fileChangedCallback(e.target.files[0]);
              }
              else {
                this.fileChangedCallback(null);
              }
            }
            else {
              this.fileChangedCallback(null);
            }
            this.value = '';
          }
        }
        this.fileSelectionEnded();
      },
      fileSelectionStarted() {
        if(UserAgentInfo.instance.isGeckoOnAndroid())
        {
          return;
        }
        this.$emit('file-selection-started');
        // a hack, don't know a way to detect if the user canceled the file dialog.
        setTimeout(() => {
          if(!this.fileChangedCalled) {
            this.fileSelectionEnded();
          }
        }, 7000);
      },
      fileSelectionEnded() {

        if(UserAgentInfo.instance.isGeckoOnAndroid())
        {
          return;
        }

        this.$emit('file-selection-ended');
        this.fileChangedCalled = false;
      }
    },
    beforeDestroy() {

      if(UserAgentInfo.instance.isGeckoOnAndroid())
      {
        return;
      }

      this.$emit('file-selection-ended');
    }
  }
</script>

<style scoped>
  .upload-btn {
    padding-left: 16px;
    padding-right: 16px;
  }
  .upload-btn input[type=file] {
    position: absolute;
    height: 0.1px;
    width: 0.1px;
    overflow: hidden;
    opacity: 0;
    z-index: -1;
  }
  .upload-btn-hover {
    cursor: pointer;
  }
</style>