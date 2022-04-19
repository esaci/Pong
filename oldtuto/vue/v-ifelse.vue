<script setup>
import { ref, onMounted } from 'vue'

let bool_enabled = ref(true);
onMounted(() => {
})

function disabling(){
  bool_enabled.value = bool_enabled.value ? false : true;
}
</script>


<template>
  <br>
  <br>
  <br>
  <br>
  <button @click="disabling"> Bouton is in {{bool_enabled}} mode!</button>
  <p v-if="bool_enabled">Now you see me because true : {{book}}</p>
  <p v-else="bool_enabled">Now you see me because false : {{book}}</p>
</template>