<script setup>

import { ref } from 'vue'

// const variable = {count : 0};
const variable = ref(0);
function addV(){
  variable.value++;
}

const obj = {batman : ref(1)};
const {batman} = obj;

</script>

<template>
  <div class="Profile">
    <button @click="addV">Appuie ! {{variable}}</button>
    <br>
    {{batman + 1}}
  </div>
</template>