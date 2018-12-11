
export class LocalStorageUtils {

  static saveIdValue(id, value, defaultValue = null) {

    if(defaultValue != null && value == defaultValue) {
      const idStr = id.toString();
      if(localStorage.getItem(idStr)) {
        localStorage.removeItem(idStr);
      }
      return;
    }
    localStorage.setItem(id.toString(), value.toString());
  }
  static saveIdValueArr(arr) {
    arr.forEach(idValue => {
      this.saveIdValue(
        idValue.id,
        idValue.value,
        idValue.defaultValue || (idValue.defaultValue === 0 ? 0 : null));
    });
  }
  static _getValueById(id) {
    
    const strValue = localStorage.getItem(id.toString());
    if(strValue === '') return strValue;
    if(strValue === null) return null;
    const value = Number(strValue);
    return isNaN(Number(value)) ? strValue : value;
  }

  static getIdValueArr(ids) {
    const idValueArr = []
    ids.forEach(id => {
      const value = this._getValueById(id);
      if(value || value === 0) idValueArr.push({ id, value });
    });
    return idValueArr;
  }

}