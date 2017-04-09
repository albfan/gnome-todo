def base_image = "waltervargas/gnome-todo"

node {
  checkout scm

  stage("Build Docker Image"){
    def image = docker.build("${base_image}:${env.GIT_BRANCH}")
    //mail to: "waltervargas@linux.com", subject: ""
  }

  stage("Run Tests"){
    image.inside {
      sh "echo Run tests"
      sh "ls -la"
    }
  }

  stage("Push image to docker registry"){
    //image.push()
    sh "echo pushing image"
  }
}
