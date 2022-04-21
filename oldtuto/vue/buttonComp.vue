<script setup>
import { ref, onMounted } from 'vue'

// j'initialise a 0 ???
const count = ref(0);

function increment(){
  count.value++;
}

onMounted(() => {
  console.log(`The initial count is ${count.value}.`)
})
</script>


<template>
  <br>
  <br>

  <button @click="increment">Count is: {{ count }} </button>

</template>
