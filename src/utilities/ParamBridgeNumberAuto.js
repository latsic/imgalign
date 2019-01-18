
export class ParamBridgeNumberAuto {
  constructor(param, paramFlag, defaultValueExternFn) {
    this.param = param;
    this.paramFlag = paramFlag;
    this.defaultValueExternFn = defaultValueExternFn;
        
    this.value = param.value;
    this.detect = paramFlag && paramFlag.value > 0 ? true : false;
  }

  setValue(value, emiterFn) {
    this.value = value;
    
    if(value || value === 0) {
      // this.param.value = +value;
      emiterFn({ id: this.param.id, value: +value });
      if(this.paramFlag) {
        // this.paramFlag.value = 0;
        emiterFn({ id: this.paramFlag.id, value: 0 });
      }
      return;
    }
    
    if(this.defaultValueExternFn) {
      const val = this.defaultValueExternFn();
      if(val === 0 || val) {
        // this.param.value = +val;
        emiterFn({ id: this.param.id, value: +val });
      }
    }
    else {
      // this.param.value = this.param.defaultValue;
      emiterFn({ id: this.param.id, value: this.param.defaultValue });
      // this.paramFlag.value = 1;
      if(this.paramFlag) {
        emiterFn({ id: this.paramFlag.id, value: 1 });
      }

    }
  }

  getValue() {
    return this.value;
  }

  placeHolder() {
    const valExtern = this.defaultValueExternFn ? this.defaultValueExternFn() : null;
    if(valExtern) {
      return 'meta: ' + valExtern;
    }
    if(this.paramFlag && this.paramFlag.value > 0) {
      return 'compute'
    }
    return this.param.defaultValue + '';
  }

}