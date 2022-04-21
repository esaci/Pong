<script setup>

import { computed, ref } from 'vue'

// const variable = {count : 0};
const variable = {var1 : ref(0), var2 : ref(0)};
function addV(){
  variable.var1.value++;
};

const negvalue = computed({
  get() {
  return (variable.var1.value + variable.var2.value);
  },
  set(valeuraset) {
    [variable.var1.value, variable.var2.value] = [0, valeuraset + variable.var2.value];
    return ;
  }
});

function addV2(){
  negvalue.value = variable.var1.value;
};

</script>

<template>
  <div class="Profile">
    <button @click="addV">Appuie ! {{variable.var1}} </button>
    <button @click="addV2">Modifie ! {{variable.var2}} </button>
    <br>
    {{negvalue}}
  </div>
</template>