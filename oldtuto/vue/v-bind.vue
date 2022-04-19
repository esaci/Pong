<script setup>
import { ref, onMounted } from 'vue'

const dynamicId = ref("myHeader");
const tab_header = ref(["myHeader", "myHeader2", "myHeader3"])
let count = ref(0);
let bool_enabled = ref(true);
onMounted(() => {
})

function modify(){
  count.value++;
  dynamicId.value = tab_header.value[count.value % tab_header.value.length];
}

function disabling(){
  bool_enabled.value = bool_enabled.value ? false : true;
}
</script>

<style>
  #myHeader {
    background-color: lightblue;
    color: black;
    padding: 40px;
    text-align: center;
  }
  #myHeader2 {
    background-color: yellow;
    color: black;
    padding: 40px;
    text-align: center;
  }
  #myHeader3 {
    background-color: green;
    color: black;
    padding: 40px;
    text-align: center;
  }
</style>

<template>
  <br>
  <br>
  <br>
  <br>
  <button @click="disabling"> Bouton is in {{bool_enabled}} mode!</button>
  <button @click="modify">myHeader{{count}}</button>
  <br>
  <br>
  <h1> Bonjour </h1>
  <h1 :id="dynamicId"> Bonjour </h1>
</template>